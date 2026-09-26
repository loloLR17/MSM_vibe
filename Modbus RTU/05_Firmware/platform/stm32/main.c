#include "stm32u5xx_hal.h"

#include "stm32_fram_storage.h"
#include "stm32_serial_transport.h"

#define TR2_BRINGUP_LED_PORT GPIOC
#define TR2_BRINGUP_LED_PIN  GPIO_PIN_7

#define TR2_FRAM_CS_PORT GPIOD
#define TR2_FRAM_CS_PIN  GPIO_PIN_14
#define TR2_FRAM_RDID_COMMAND 0x9FU
#define TR2_FRAM_RDID_SIZE 4U
#define TR2_FRAM_SPI_TIMEOUT_MS 10U

static const uint8_t tr2_fram_expected_device_id[TR2_FRAM_RDID_SIZE] = {
    0x04U, 0x7FU, 0x48U, 0x03U
};

static SPI_HandleTypeDef hspi1;

volatile HAL_StatusTypeDef tr2_fram_rdid_status = HAL_ERROR;
volatile uint8_t tr2_fram_device_id[TR2_FRAM_RDID_SIZE] = {0U};
volatile uint8_t tr2_fram_device_id_matches = 0U;

static uint8_t tr2_fram_d2_candidate[TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE];

volatile uint32_t tr2_fram_d2_storage_init_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d2_geometry_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d2_media_init_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d2_recover_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d2_recovery_status = (uint32_t)TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;
volatile uint64_t tr2_fram_d2_generation = UINT64_C(0);
volatile uint8_t tr2_fram_d2_active_image = 0xFFU;

/*
 * D2-B qualification gate.
 *
 * Keep this at 0 for normal/read-only boots.  Set it to 1 only for the
 * explicitly destructive D2-B qualification build.  Formatting is attempted
 * only when recovery has positively classified the physical medium EMPTY.
 */
#define TR2_FRAM_D2B_ALLOW_FORMAT_EMPTY 0U
#define TR2_FRAM_D2B_RESET_METADATA_FOR_QUALIFICATION 0U

volatile uint32_t tr2_fram_d2b_reset_attempted = 0U;
volatile uint32_t tr2_fram_d2b_reset_result = (uint32_t)TR2_ERROR_INTERNAL;

volatile uint32_t tr2_fram_d2b_format_attempted = 0U;
volatile uint32_t tr2_fram_d2b_format_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d2b_post_format_recover_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d2b_post_format_status = (uint32_t)TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;
volatile uint64_t tr2_fram_d2b_post_format_generation = UINT64_C(0);
volatile uint8_t tr2_fram_d2b_post_format_active_image = 0xFFU;

/*
 * D2-C qualification: after a normal VALID recovery, modify one byte through
 * the frozen PersistentMedia interface and commit once.  The gate is kept at
 * 0 until the pre-commit physical state has been observed.
 */
#define TR2_FRAM_D2C_ALLOW_COMMIT 0U
#define TR2_FRAM_D2C_TEST_OFFSET UINT32_C(0)
#define TR2_FRAM_D2C_TEST_VALUE UINT8_C(0xA5)

/*
 * D2-D1 physical power-loss qualification.
 *
 * When armed, the physical-storage adapter delegates every operation except
 * the generation-3 inactive-image payload write.  That write is deliberately
 * limited to a prefix, then the CPU latches a visible cut-power point and
 * stops.  H3d2 itself remains unchanged.
 */
#define TR2_FRAM_D2D1_ALLOW_PARTIAL_PAYLOAD 0U
#define TR2_FRAM_D2D1_TEST_VALUE UINT8_C(0x5A)
#define TR2_FRAM_D2D1_PARTIAL_SIZE ((size_t)4096U)

volatile uint32_t tr2_fram_d2d1_write_attempted = 0U;
volatile uint32_t tr2_fram_d2d1_write_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d2d1_commit_attempted = 0U;
volatile uint32_t tr2_fram_d2d1_cut_point_reached = 0U;

/*
 * D2-D2 physical power-loss qualification.
 *
 * When armed, the adapter lets the complete generation-3 image-A payload
 * reach the real FRAM, then latches the cut-power point before H3d2 can issue
 * the final image-header write.  D2-D1 remains independently disarmed.
 */
#define TR2_FRAM_D2D2_ALLOW_COMPLETE_PAYLOAD 0U

volatile uint32_t tr2_fram_d2d2_write_attempted = 0U;
volatile uint32_t tr2_fram_d2d2_write_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d2d2_commit_attempted = 0U;
volatile uint32_t tr2_fram_d2d2_cut_point_reached = 0U;

/*
 * D2-D4 physical power-loss qualification.
 *
 * Starting from the physically observed gen3/A/0x5A authority, allow H3d2
 * to build and validate candidate gen4 in inactive image B, then delegate
 * the complete opposite-superblock publication write to the real FRAM.
 * Stop immediately after that physical write returns TR2_OK and before
 * returning control to H3d2.  No retry or repair is performed.
 */
#define TR2_FRAM_D2D4_ALLOW_PUBLISHED_CANDIDATE 1U
#define TR2_FRAM_D2D4_TEST_VALUE UINT8_C(0xA6)

volatile uint32_t tr2_fram_d2d4_write_attempted = 0U;
volatile uint32_t tr2_fram_d2d4_write_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d2d4_commit_attempted = 0U;
volatile uint32_t tr2_fram_d2d4_payload_completed = 0U;
volatile uint32_t tr2_fram_d2d4_header_completed = 0U;
volatile uint32_t tr2_fram_d2d4_publication_completed = 0U;
volatile uint32_t tr2_fram_d2d4_cut_point_reached = 0U;

#if TR2_FRAM_D2D4_ALLOW_PUBLISHED_CANDIDATE
typedef struct {
    TransactionalImagePhysicalStorage underlying;
    uint32_t target_payload_offset;
    uint32_t target_header_offset;
    uint32_t target_superblock_offset;
} Tr2D2d4PhysicalStorage;

static Tr2Result D2d4PhysicalRead(
    void *context, uint32_t offset, void *buffer, size_t size)
{
    Tr2D2d4PhysicalStorage *adapter = (Tr2D2d4PhysicalStorage *)context;
    return adapter->underlying.read(
        adapter->underlying.context, offset, buffer, size);
}

static Tr2Result D2d4PhysicalWrite(
    void *context, uint32_t offset, const void *buffer, size_t size)
{
    Tr2D2d4PhysicalStorage *adapter = (Tr2D2d4PhysicalStorage *)context;
    Tr2Result result = adapter->underlying.write(
        adapter->underlying.context, offset, buffer, size);

    if (result != TR2_OK) {
        return result;
    }

    if ((offset == adapter->target_payload_offset) &&
        (size == TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)) {
        tr2_fram_d2d4_payload_completed = 1U;
    } else if ((offset == adapter->target_header_offset) &&
               (size == TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE) &&
               (tr2_fram_d2d4_payload_completed == 1U)) {
        tr2_fram_d2d4_header_completed = 1U;
    } else if ((offset == adapter->target_superblock_offset) &&
               (size == TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE) &&
               (tr2_fram_d2d4_payload_completed == 1U) &&
               (tr2_fram_d2d4_header_completed == 1U)) {
        tr2_fram_d2d4_publication_completed = 1U;
        tr2_fram_d2d4_cut_point_reached = 1U;
        HAL_GPIO_WritePin(
            TR2_BRINGUP_LED_PORT, TR2_BRINGUP_LED_PIN, GPIO_PIN_SET);
        __disable_irq();
        for (;;) {
            /* Real publication is complete; operator removes board power. */
        }
    }

    return TR2_OK;
}
#endif

/*
 * D2-D3 physical power-loss qualification.
 *
 * The adapter is configured only after recovery, from the authority actually
 * observed on FRAM.  It therefore follows the H3d2 rule "publish through the
 * opposite superblock" instead of assuming a particular generation/copy.
 */
#define TR2_FRAM_D2D3_ALLOW_FINALIZED_IMAGE 0U
#define TR2_FRAM_D2D3_TEST_VALUE UINT8_C(0xA6)

volatile uint32_t tr2_fram_d2d3_write_attempted = 0U;
volatile uint32_t tr2_fram_d2d3_write_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d2d3_commit_attempted = 0U;
volatile uint32_t tr2_fram_d2d3_payload_completed = 0U;
volatile uint32_t tr2_fram_d2d3_header_completed = 0U;
volatile uint32_t tr2_fram_d2d3_cut_point_reached = 0U;

#if TR2_FRAM_D2D3_ALLOW_FINALIZED_IMAGE
typedef struct {
    TransactionalImagePhysicalStorage underlying;
    uint32_t target_payload_offset;
    uint32_t target_header_offset;
    uint32_t target_superblock_offset;
} Tr2D2d3PhysicalStorage;

static Tr2Result D2d3PhysicalRead(
    void *context, uint32_t offset, void *buffer, size_t size)
{
    Tr2D2d3PhysicalStorage *adapter = (Tr2D2d3PhysicalStorage *)context;
    return adapter->underlying.read(
        adapter->underlying.context, offset, buffer, size);
}

static Tr2Result D2d3PhysicalWrite(
    void *context, uint32_t offset, const void *buffer, size_t size)
{
    Tr2D2d3PhysicalStorage *adapter = (Tr2D2d3PhysicalStorage *)context;
    Tr2Result result;

    if ((offset == adapter->target_superblock_offset) &&
        (size == TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE) &&
        (tr2_fram_d2d3_payload_completed == 1U) &&
        (tr2_fram_d2d3_header_completed == 1U)) {
        tr2_fram_d2d3_cut_point_reached = 1U;
        HAL_GPIO_WritePin(
            TR2_BRINGUP_LED_PORT, TR2_BRINGUP_LED_PIN, GPIO_PIN_SET);
        __disable_irq();
        for (;;) {
            /* Publication write intentionally not delegated. */
        }
    }

    result = adapter->underlying.write(
        adapter->underlying.context, offset, buffer, size);

    if (result == TR2_OK) {
        if ((offset == adapter->target_payload_offset) &&
            (size == TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)) {
            tr2_fram_d2d3_payload_completed = 1U;
        } else if ((offset == adapter->target_header_offset) &&
                   (size == TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE) &&
                   (tr2_fram_d2d3_payload_completed == 1U)) {
            tr2_fram_d2d3_header_completed = 1U;
        }
    }

    return result;
}
#endif

#if TR2_FRAM_D2D2_ALLOW_COMPLETE_PAYLOAD
typedef struct {
    TransactionalImagePhysicalStorage underlying;
    uint32_t target_payload_offset;
} Tr2D2d2PhysicalStorage;

static Tr2Result D2d2PhysicalRead(
    void *context, uint32_t offset, void *buffer, size_t size)
{
    Tr2D2d2PhysicalStorage *adapter = (Tr2D2d2PhysicalStorage *)context;
    return adapter->underlying.read(
        adapter->underlying.context, offset, buffer, size);
}

static Tr2Result D2d2PhysicalWrite(
    void *context, uint32_t offset, const void *buffer, size_t size)
{
    Tr2D2d2PhysicalStorage *adapter = (Tr2D2d2PhysicalStorage *)context;

    if ((offset == adapter->target_payload_offset) &&
        (size == TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)) {
        Tr2Result result = adapter->underlying.write(
            adapter->underlying.context, offset, buffer, size);

        if (result != TR2_OK) {
            return result;
        }

        tr2_fram_d2d2_cut_point_reached = 1U;
        HAL_GPIO_WritePin(
            TR2_BRINGUP_LED_PORT, TR2_BRINGUP_LED_PIN, GPIO_PIN_SET);
        __disable_irq();
        for (;;) {
            /* Operator now removes board power for the physical test. */
        }
    }

    return adapter->underlying.write(
        adapter->underlying.context, offset, buffer, size);
}
#endif

#if TR2_FRAM_D2D1_ALLOW_PARTIAL_PAYLOAD
typedef struct {
    TransactionalImagePhysicalStorage underlying;
    uint32_t target_payload_offset;
} Tr2D2d1PhysicalStorage;

static Tr2Result D2d1PhysicalRead(
    void *context, uint32_t offset, void *buffer, size_t size)
{
    Tr2D2d1PhysicalStorage *adapter = (Tr2D2d1PhysicalStorage *)context;
    return adapter->underlying.read(
        adapter->underlying.context, offset, buffer, size);
}

static Tr2Result D2d1PhysicalWrite(
    void *context, uint32_t offset, const void *buffer, size_t size)
{
    Tr2D2d1PhysicalStorage *adapter = (Tr2D2d1PhysicalStorage *)context;

    if ((offset == adapter->target_payload_offset) &&
        (size == TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)) {
        Tr2Result result = adapter->underlying.write(
            adapter->underlying.context,
            offset,
            buffer,
            TR2_FRAM_D2D1_PARTIAL_SIZE);

        if (result != TR2_OK) {
            return result;
        }

        tr2_fram_d2d1_cut_point_reached = 1U;
        HAL_GPIO_WritePin(
            TR2_BRINGUP_LED_PORT, TR2_BRINGUP_LED_PIN, GPIO_PIN_SET);
        __disable_irq();
        for (;;) {
            /* Operator now removes board power for the physical test. */
        }
    }

    return adapter->underlying.write(
        adapter->underlying.context, offset, buffer, size);
}
#endif

volatile uint32_t tr2_fram_d2c_write_attempted = 0U;
volatile uint32_t tr2_fram_d2c_write_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d2c_commit_attempted = 0U;
volatile uint32_t tr2_fram_d2c_commit_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d2c_post_commit_recover_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d2c_post_commit_status = (uint32_t)TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;
volatile uint64_t tr2_fram_d2c_post_commit_generation = UINT64_C(0);
volatile uint8_t tr2_fram_d2c_post_commit_active_image = 0xFFU;
volatile uint8_t tr2_fram_d2c_readback = 0U;
volatile uint32_t tr2_fram_d2c_readback_result = (uint32_t)TR2_ERROR_INTERNAL;

static void SystemClock_Config(void);
static void SystemPower_Config(void);
static void BringupLed_Init(void);
static void FramSpi_Init(void);
static HAL_StatusTypeDef Fram_ReadDeviceId(uint8_t device_id[TR2_FRAM_RDID_SIZE]);
static void Error_Handler(void);

void HAL_MspInit(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
}

int main(void)
{
    SerialTransport serial_transport;

    HAL_Init();
    SystemClock_Config();
    SystemPower_Config();
    BringupLed_Init();
    FramSpi_Init();

    {
        uint8_t device_id[TR2_FRAM_RDID_SIZE] = {0U};
        uint8_t matches = 1U;

        tr2_fram_rdid_status = Fram_ReadDeviceId(device_id);

        for (uint32_t i = 0U; i < TR2_FRAM_RDID_SIZE; ++i) {
            tr2_fram_device_id[i] = device_id[i];
            if (device_id[i] != tr2_fram_expected_device_id[i]) {
                matches = 0U;
            }
        }

        tr2_fram_device_id_matches =
            (tr2_fram_rdid_status == HAL_OK) ? matches : 0U;
    }

    {
        Stm32FramStorage fram_storage;
        TransactionalImagePhysicalStorage physical;
        TransactionalImageGeometry geometry;
        TransactionalImageMedia media;
        TransactionalImageRecoveryResult recovery = {
            .status = TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE,
            .generation = UINT64_C(0),
            .active_image = 0xFFU
        };

        tr2_fram_d2_storage_init_result = (uint32_t)stm32_fram_storage_init(
            &fram_storage,
            &hspi1,
            TR2_FRAM_CS_PORT,
            TR2_FRAM_CS_PIN,
            TR2_FRAM_SPI_TIMEOUT_MS);

        physical = stm32_fram_storage_physical(&fram_storage);
        geometry = transactional_image_geometry_qualification_profile();
        tr2_fram_d2_geometry_result =
            (uint32_t)transactional_image_geometry_validate(&geometry);

#if TR2_FRAM_D2D1_ALLOW_PARTIAL_PAYLOAD
        {
            static Tr2D2d1PhysicalStorage d2d1_storage;
            TransactionalImagePhysicalStorage qualified_physical;

            d2d1_storage.underlying = physical;
            d2d1_storage.target_payload_offset =
                geometry.image_a_base +
                TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE;

            qualified_physical.context = &d2d1_storage;
            qualified_physical.read = D2d1PhysicalRead;
            qualified_physical.write = D2d1PhysicalWrite;
            physical = qualified_physical;
        }
#endif


#if TR2_FRAM_D2D2_ALLOW_COMPLETE_PAYLOAD
        {
            static Tr2D2d2PhysicalStorage d2d2_storage;
            TransactionalImagePhysicalStorage qualified_physical;

            d2d2_storage.underlying = physical;
            d2d2_storage.target_payload_offset =
                geometry.image_a_base +
                TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE;

            qualified_physical.context = &d2d2_storage;
            qualified_physical.read = D2d2PhysicalRead;
            qualified_physical.write = D2d2PhysicalWrite;
            physical = qualified_physical;
        }
#endif



        if ((tr2_fram_d2_storage_init_result == (uint32_t)TR2_OK) &&
            (tr2_fram_d2_geometry_result == (uint32_t)TR2_OK)) {
            tr2_fram_d2_media_init_result = (uint32_t)transactional_image_media_init(
                &media,
                &physical,
                &geometry,
                tr2_fram_d2_candidate,
                sizeof(tr2_fram_d2_candidate));
        }

#if TR2_FRAM_D2B_RESET_METADATA_FOR_QUALIFICATION
        /*
         * D2-B destructive qualification only: clear exactly the four
         * H3d2 publication/header records so the subsequent recovery must
         * classify the medium EMPTY.  Payload areas are deliberately left
         * untouched because EMPTY classification is defined by these records.
         *
         * This gate MUST be returned to 0 immediately after the qualification
         * boot; while it is 1, every reset deliberately destroys publication
         * metadata.
         */
        if (tr2_fram_d2_media_init_result == (uint32_t)TR2_OK) {
            static const uint8_t empty_record[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE] = {0U};
            Tr2Result reset_result = TR2_OK;

            tr2_fram_d2b_reset_attempted = 1U;

            reset_result = physical.write(
                physical.context, geometry.superblock_a_base,
                empty_record, sizeof(empty_record));
            if (reset_result == TR2_OK) {
                reset_result = physical.write(
                    physical.context, geometry.superblock_b_base,
                    empty_record, sizeof(empty_record));
            }
            if (reset_result == TR2_OK) {
                reset_result = physical.write(
                    physical.context, geometry.image_a_base,
                    empty_record, TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE);
            }
            if (reset_result == TR2_OK) {
                reset_result = physical.write(
                    physical.context, geometry.image_b_base,
                    empty_record, TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE);
            }

            tr2_fram_d2b_reset_result = (uint32_t)reset_result;
        }
#endif

        if ((tr2_fram_d2_media_init_result == (uint32_t)TR2_OK) &&
            (tr2_fram_d2b_reset_attempted == 0U ||
             tr2_fram_d2b_reset_result == (uint32_t)TR2_OK)) {
            tr2_fram_d2_recover_result =
                (uint32_t)transactional_image_media_recover(&media, &recovery);
        }

        if (tr2_fram_d2_recover_result == (uint32_t)TR2_OK) {
            tr2_fram_d2_recovery_status = (uint32_t)recovery.status;
            tr2_fram_d2_generation = recovery.generation;
            tr2_fram_d2_active_image = recovery.active_image;
        }

#if TR2_FRAM_D2B_ALLOW_FORMAT_EMPTY
        /*
         * Destructive qualification is deliberately gated by the observed
         * EMPTY state.  Never format VALID, CORRUPTED, UNSUPPORTED or
         * UNAVAILABLE media automatically.
         */
        if ((tr2_fram_d2_recover_result == (uint32_t)TR2_OK) &&
            (recovery.status == TRANSACTIONAL_IMAGE_RECOVERY_EMPTY)) {
            TransactionalImageRecoveryResult post_format = {
                .status = TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE,
                .generation = UINT64_C(0),
                .active_image = 0xFFU
            };

            tr2_fram_d2b_format_attempted = 1U;
            tr2_fram_d2b_format_result =
                (uint32_t)transactional_image_media_format_empty(&media);

            if (tr2_fram_d2b_format_result == (uint32_t)TR2_OK) {
                tr2_fram_d2b_post_format_recover_result =
                    (uint32_t)transactional_image_media_recover(&media, &post_format);

                if (tr2_fram_d2b_post_format_recover_result == (uint32_t)TR2_OK) {
                    tr2_fram_d2b_post_format_status = (uint32_t)post_format.status;
                    tr2_fram_d2b_post_format_generation = post_format.generation;
                    tr2_fram_d2b_post_format_active_image = post_format.active_image;
                }
            }
        }
#endif

#if TR2_FRAM_D2C_ALLOW_COMMIT
        /*
         * Execute exactly one transactional mutation only from the known
         * D2-B baseline.  Requiring generation 1 / image A makes subsequent
         * boots inert after a successful commit to generation 2 / image B.
         */
        if ((tr2_fram_d2_recover_result == (uint32_t)TR2_OK) &&
            (recovery.status == TRANSACTIONAL_IMAGE_RECOVERY_VALID) &&
            (recovery.generation == UINT64_C(1)) &&
            (recovery.active_image == 0U)) {
            PersistentMedia *persistent = transactional_image_media_interface(&media);
            uint8_t value = TR2_FRAM_D2C_TEST_VALUE;

            if (persistent != NULL) {
                tr2_fram_d2c_write_attempted = 1U;
                tr2_fram_d2c_write_result = (uint32_t)persistent->write(
                    persistent->context,
                    TR2_FRAM_D2C_TEST_OFFSET,
                    &value,
                    sizeof(value));

                if (tr2_fram_d2c_write_result == (uint32_t)TR2_OK) {
                    TransactionalImageRecoveryResult post_commit = {
                        .status = TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE,
                        .generation = UINT64_C(0),
                        .active_image = 0xFFU
                    };

                    tr2_fram_d2c_commit_attempted = 1U;
                    tr2_fram_d2c_commit_result =
                        (uint32_t)persistent->commit(persistent->context);

                    if (tr2_fram_d2c_commit_result == (uint32_t)TR2_OK) {
                        tr2_fram_d2c_post_commit_recover_result =
                            (uint32_t)transactional_image_media_recover(
                                &media, &post_commit);

                        if (tr2_fram_d2c_post_commit_recover_result == (uint32_t)TR2_OK) {
                            tr2_fram_d2c_post_commit_status =
                                (uint32_t)post_commit.status;
                            tr2_fram_d2c_post_commit_generation =
                                post_commit.generation;
                            tr2_fram_d2c_post_commit_active_image =
                                post_commit.active_image;

                            tr2_fram_d2c_readback_result =
                                (uint32_t)persistent->read(
                                    persistent->context,
                                    TR2_FRAM_D2C_TEST_OFFSET,
                                    (void *)&tr2_fram_d2c_readback,
                                    sizeof(tr2_fram_d2c_readback));
                        }
                    }
                }
            }
        }
#endif

#if TR2_FRAM_D2D1_ALLOW_PARTIAL_PAYLOAD
        /*
         * D2-D1 starts only from the physically observed D2-C baseline.
         * The commit cannot return after the target partial payload write:
         * D2d1PhysicalWrite() latches the cut point and stops the CPU.
         */
        if ((tr2_fram_d2_recover_result == (uint32_t)TR2_OK) &&
            (recovery.status == TRANSACTIONAL_IMAGE_RECOVERY_VALID) &&
            (recovery.generation == UINT64_C(3)) &&
            (recovery.active_image == 0U) &&
            (tr2_fram_d2_candidate[TR2_FRAM_D2C_TEST_OFFSET] ==
             TR2_FRAM_D2D1_TEST_VALUE)) {
            PersistentMedia *persistent =
                transactional_image_media_interface(&media);
            uint8_t value = TR2_FRAM_D2D3_TEST_VALUE;

            if (persistent != NULL) {
                tr2_fram_d2d1_write_attempted = 1U;
                tr2_fram_d2d1_write_result = (uint32_t)persistent->write(
                    persistent->context,
                    TR2_FRAM_D2C_TEST_OFFSET,
                    &value,
                    sizeof(value));

                if (tr2_fram_d2d1_write_result == (uint32_t)TR2_OK) {
                    tr2_fram_d2d1_commit_attempted = 1U;
                    (void)persistent->commit(persistent->context);
                }
            }
        }
#endif

#if TR2_FRAM_D2D2_ALLOW_COMPLETE_PAYLOAD
        /*
         * D2-D2 starts only from the explicitly observed generation-2/B
         * baseline recovered after D2-D1.  The adapter lets the complete
         * payload write finish, then stops before the final image header.
         */
        if ((tr2_fram_d2_recover_result == (uint32_t)TR2_OK) &&
            (recovery.status == TRANSACTIONAL_IMAGE_RECOVERY_VALID) &&
            (recovery.generation == UINT64_C(2)) &&
            (recovery.active_image == 1U) &&
            (tr2_fram_d2_candidate[TR2_FRAM_D2C_TEST_OFFSET] ==
             TR2_FRAM_D2C_TEST_VALUE)) {
            PersistentMedia *persistent =
                transactional_image_media_interface(&media);
            uint8_t value = TR2_FRAM_D2D1_TEST_VALUE;

            if (persistent != NULL) {
                tr2_fram_d2d2_write_attempted = 1U;
                tr2_fram_d2d2_write_result = (uint32_t)persistent->write(
                    persistent->context,
                    TR2_FRAM_D2C_TEST_OFFSET,
                    &value,
                    sizeof(value));

                if (tr2_fram_d2d2_write_result == (uint32_t)TR2_OK) {
                    tr2_fram_d2d2_commit_attempted = 1U;
                    (void)persistent->commit(persistent->context);
                }
            }
        }
#endif

#if TR2_FRAM_D2D4_ALLOW_PUBLISHED_CANDIDATE
        /*
         * Derive inactive image and publication copy from the authority
         * actually recovered from FRAM, then recover once more through the
         * transparent adapter before starting the destructive scenario.
         */
        if ((tr2_fram_d2_recover_result == (uint32_t)TR2_OK) &&
            (recovery.status == TRANSACTIONAL_IMAGE_RECOVERY_VALID)) {
            static Tr2D2d4PhysicalStorage d2d4_storage;
            TransactionalImagePhysicalStorage qualified_physical;
            TransactionalImageRecoveryResult qualified_recovery = {
                .status = TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE,
                .generation = UINT64_C(0),
                .active_image = 0xFFU
            };
            uint8_t inactive_image = (uint8_t)(1U - recovery.active_image);
            uint8_t publication_copy = (uint8_t)(1U - media.active_superblock);

            d2d4_storage.underlying = physical;
            d2d4_storage.target_payload_offset =
                (inactive_image == 0U ? geometry.image_a_base : geometry.image_b_base) +
                TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE;
            d2d4_storage.target_header_offset =
                inactive_image == 0U ? geometry.image_a_base : geometry.image_b_base;
            d2d4_storage.target_superblock_offset =
                publication_copy == 0U ? geometry.superblock_a_base : geometry.superblock_b_base;

            qualified_physical.context = &d2d4_storage;
            qualified_physical.read = D2d4PhysicalRead;
            qualified_physical.write = D2d4PhysicalWrite;

            if (transactional_image_media_init(
                    &media, &qualified_physical, &geometry,
                    tr2_fram_d2_candidate, sizeof(tr2_fram_d2_candidate)) == TR2_OK) {
                (void)transactional_image_media_recover(&media, &qualified_recovery);
            }
        }
#endif

#if TR2_FRAM_D2D3_ALLOW_FINALIZED_IMAGE
        /*
         * Bind the qualification adapter only after recovery so both inactive
         * image and opposite publication copy are derived from observed
         * authority.  Re-initialize then recover through the adapter; reads
         * delegate unchanged, so this second recovery is non-destructive.
         */
        if ((tr2_fram_d2_recover_result == (uint32_t)TR2_OK) &&
            (recovery.status == TRANSACTIONAL_IMAGE_RECOVERY_VALID)) {
            static Tr2D2d3PhysicalStorage d2d3_storage;
            TransactionalImagePhysicalStorage qualified_physical;
            TransactionalImageRecoveryResult qualified_recovery = {
                .status = TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE,
                .generation = UINT64_C(0),
                .active_image = 0xFFU
            };
            uint8_t inactive_image = (uint8_t)(1U - recovery.active_image);
            uint8_t publication_copy = (uint8_t)(1U - media.active_superblock);

            d2d3_storage.underlying = physical;
            d2d3_storage.target_payload_offset =
                (inactive_image == 0U ? geometry.image_a_base : geometry.image_b_base) +
                TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE;
            d2d3_storage.target_header_offset =
                inactive_image == 0U ? geometry.image_a_base : geometry.image_b_base;
            d2d3_storage.target_superblock_offset =
                publication_copy == 0U ? geometry.superblock_a_base : geometry.superblock_b_base;

            qualified_physical.context = &d2d3_storage;
            qualified_physical.read = D2d3PhysicalRead;
            qualified_physical.write = D2d3PhysicalWrite;

            if (transactional_image_media_init(
                    &media, &qualified_physical, &geometry,
                    tr2_fram_d2_candidate, sizeof(tr2_fram_d2_candidate)) == TR2_OK) {
                (void)transactional_image_media_recover(&media, &qualified_recovery);
            }
        }
#endif

#if TR2_FRAM_D2D4_ALLOW_PUBLISHED_CANDIDATE
        /*
         * D2-D4 starts only from the observed gen3/A/0x5A authority.
         * The adapter stops only after the complete publication write has
         * physically returned TR2_OK.
         */
        if ((tr2_fram_d2_recover_result == (uint32_t)TR2_OK) &&
            (recovery.status == TRANSACTIONAL_IMAGE_RECOVERY_VALID) &&
            (recovery.generation == UINT64_C(3)) &&
            (recovery.active_image == 0U) &&
            (tr2_fram_d2_candidate[TR2_FRAM_D2C_TEST_OFFSET] ==
             TR2_FRAM_D2D1_TEST_VALUE)) {
            PersistentMedia *persistent =
                transactional_image_media_interface(&media);
            uint8_t value = TR2_FRAM_D2D4_TEST_VALUE;

            if (persistent != NULL) {
                tr2_fram_d2d4_write_attempted = 1U;
                tr2_fram_d2d4_write_result = (uint32_t)persistent->write(
                    persistent->context,
                    TR2_FRAM_D2C_TEST_OFFSET,
                    &value,
                    sizeof(value));

                if (tr2_fram_d2d4_write_result == (uint32_t)TR2_OK) {
                    tr2_fram_d2d4_commit_attempted = 1U;
                    (void)persistent->commit(persistent->context);
                }
            }
        }
#endif

#if TR2_FRAM_D2D3_ALLOW_FINALIZED_IMAGE
        /*
         * D2-D3 starts only from the physically observed generation-3/A
         * authority with payload[0] == 0x5A. Reaching the adapter cut point
         * proves that the generation-4 inactive-image payload and final header
         * writes returned TR2_OK, image validation succeeded, and H3d2 then
         * attempted publication through the opposite superblock. The
         * publication write itself is not delegated.
         */
        if ((tr2_fram_d2_recover_result == (uint32_t)TR2_OK) &&
            (recovery.status == TRANSACTIONAL_IMAGE_RECOVERY_VALID) &&
            (recovery.generation == UINT64_C(3)) &&
            (recovery.active_image == 0U) &&
            (tr2_fram_d2_candidate[TR2_FRAM_D2C_TEST_OFFSET] ==
             TR2_FRAM_D2D1_TEST_VALUE)) {
            PersistentMedia *persistent =
                transactional_image_media_interface(&media);
            uint8_t value = TR2_FRAM_D2D3_TEST_VALUE;

            if (persistent != NULL) {
                tr2_fram_d2d3_write_attempted = 1U;
                tr2_fram_d2d3_write_result = (uint32_t)persistent->write(
                    persistent->context,
                    TR2_FRAM_D2C_TEST_OFFSET,
                    &value,
                    sizeof(value));

                if (tr2_fram_d2d3_write_result == (uint32_t)TR2_OK) {
                    tr2_fram_d2d3_commit_attempted = 1U;
                    (void)persistent->commit(persistent->context);
                }
            }
        }
#endif
    }

    if (stm32_serial_transport_init(&serial_transport) != TR2_OK) {
        Error_Handler();
    }

    if (serial_transport_start_receive(&serial_transport) != TR2_OK) {
        Error_Handler();
    }

    for (;;) {
        HAL_GPIO_TogglePin(TR2_BRINGUP_LED_PORT, TR2_BRINGUP_LED_PIN);
        HAL_Delay(250U);
    }
}

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
        Error_Handler();
    }

    osc.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    osc.MSIState = RCC_MSI_ON;
    osc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    osc.MSIClockRange = RCC_MSIRANGE_4;
    osc.PLL.PLLState = RCC_PLL_ON;
    osc.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    osc.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
    osc.PLL.PLLM = 1U;
    osc.PLL.PLLN = 80U;
    osc.PLL.PLLP = 2U;
    osc.PLL.PLLQ = 2U;
    osc.PLL.PLLR = 2U;
    osc.PLL.PLLRGE = RCC_PLLVCIRANGE_0;
    osc.PLL.PLLFRACN = 0U;

    if (HAL_RCC_OscConfig(&osc) != HAL_OK) {
        Error_Handler();
    }

    clk.ClockType = RCC_CLOCKTYPE_HCLK |
                    RCC_CLOCKTYPE_SYSCLK |
                    RCC_CLOCKTYPE_PCLK1 |
                    RCC_CLOCKTYPE_PCLK2 |
                    RCC_CLOCKTYPE_PCLK3;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV1;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;
    clk.APB3CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_4) != HAL_OK) {
        Error_Handler();
    }
}

static void SystemPower_Config(void)
{
    HAL_PWREx_DisableUCPDDeadBattery();

    if (HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY) != HAL_OK) {
        Error_Handler();
    }
}

static void BringupLed_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();

    HAL_GPIO_WritePin(TR2_BRINGUP_LED_PORT, TR2_BRINGUP_LED_PIN, GPIO_PIN_RESET);

    gpio.Pin = TR2_BRINGUP_LED_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TR2_BRINGUP_LED_PORT, &gpio);
}

static void FramSpi_Init(void)
{
    GPIO_InitTypeDef gpio = {0};
    RCC_PeriphCLKInitTypeDef periph_clk = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    HAL_GPIO_WritePin(TR2_FRAM_CS_PORT, TR2_FRAM_CS_PIN, GPIO_PIN_SET);

    gpio.Pin = TR2_FRAM_CS_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TR2_FRAM_CS_PORT, &gpio);

    periph_clk.PeriphClockSelection = RCC_PERIPHCLK_SPI1;
    periph_clk.Spi1ClockSelection = RCC_SPI1CLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&periph_clk) != HAL_OK) {
        Error_Handler();
    }

    __HAL_RCC_SPI1_CLK_ENABLE();

    gpio.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &gpio);

    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 0x7U;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_08DATA;
    hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
    hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;
    hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    hspi1.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
    hspi1.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;

    if (HAL_SPI_Init(&hspi1) != HAL_OK) {
        Error_Handler();
    }
}

static HAL_StatusTypeDef Fram_ReadDeviceId(uint8_t device_id[TR2_FRAM_RDID_SIZE])
{
    uint8_t command = TR2_FRAM_RDID_COMMAND;
    HAL_StatusTypeDef status;

    HAL_GPIO_WritePin(TR2_FRAM_CS_PORT, TR2_FRAM_CS_PIN, GPIO_PIN_RESET);

    status = HAL_SPI_Transmit(&hspi1, &command, 1U, TR2_FRAM_SPI_TIMEOUT_MS);
    if (status == HAL_OK) {
        status = HAL_SPI_Receive(&hspi1,
                                 device_id,
                                 TR2_FRAM_RDID_SIZE,
                                 TR2_FRAM_SPI_TIMEOUT_MS);
    }

    HAL_GPIO_WritePin(TR2_FRAM_CS_PORT, TR2_FRAM_CS_PIN, GPIO_PIN_SET);

    return status;
}

static void Error_Handler(void)
{
    __disable_irq();
    for (;;) {
    }
}
