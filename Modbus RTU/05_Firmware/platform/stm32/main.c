#include "stm32u5xx_hal.h"

#include <string.h>

#include "stm32_fram_storage.h"
#include "stm32_iis3dwb_vibration_source.h"
#include "iis3dwb_diag_window.h"
#include "stm32_serial_transport.h"
#include "iis3dwb_diag_modbus.h"
#include "stm32_runtime_platform.h"
#include "stm32_sdmmc_bulk_media.h"

#include "tr2/persistence/campaign_data_store_bulk.h"

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
static SPI_HandleTypeDef hspi3;
static SD_HandleTypeDef hsd2;

/*
 * H3h-C microSD physical qualification instrumentation.
 * H3h-C2b is explicitly destructive on the sacrificial test card: it performs
 * one bounded raw multi-block write followed by read-back verification.
 * No filesystem, format or erase operation is issued here.
 */
volatile uint32_t tr2_sdmmc2_init_attempted = 0U;
volatile uint32_t tr2_sdmmc2_init_status = (uint32_t)HAL_ERROR;
volatile uint32_t tr2_sdmmc2_error_code = 0U;
volatile uint32_t tr2_sdmmc2_kernel_clock_hz = 0U;
volatile uint32_t tr2_sdmmc2_last_cmd = 0U;
volatile uint32_t tr2_sdmmc2_sta = 0U;
volatile uint32_t tr2_sdmmc2_resp1 = 0U;
volatile uint32_t tr2_sdmmc2_clkcr = 0U;
volatile uint32_t tr2_sdmmc2_power = 0U;
volatile uint32_t tr2_sdmmc2_pre_cmd16_card_type = 0U;
volatile uint32_t tr2_sdmmc2_pre_cmd16_card_version = 0U;
volatile uint32_t tr2_sdmmc2_pre_cmd16_card_class = 0U;
volatile uint32_t tr2_sdmmc2_pre_cmd16_rca = 0U;
volatile uint32_t tr2_sdmmc2_pre_cmd16_block_nbr = 0U;
volatile uint32_t tr2_sdmmc2_pre_cmd16_block_size = 0U;
volatile uint32_t tr2_sdmmc2_pre_cmd16_log_block_nbr = 0U;
volatile uint32_t tr2_sdmmc2_pre_cmd16_log_block_size = 0U;
volatile uint32_t tr2_sdmmc2_stage = 0U;
volatile uint32_t tr2_sdmmc2_card_info_status = (uint32_t)HAL_ERROR;
volatile uint32_t tr2_sdmmc2_card_type = 0U;
volatile uint32_t tr2_sdmmc2_card_version = 0U;
volatile uint32_t tr2_sdmmc2_card_class = 0U;
volatile uint32_t tr2_sdmmc2_relative_card_address = 0U;
volatile uint32_t tr2_sdmmc2_log_block_nbr = 0U;
volatile uint32_t tr2_sdmmc2_log_block_size = 0U;
volatile uint32_t tr2_sdmmc2_block_nbr = 0U;
volatile uint32_t tr2_sdmmc2_block_size = 0U;

/*
 * H3h-E1-P physical qualification of the CampaignBulkMedia SDMMC adapter.
 * The bounded destructive area stays inside the already-authorized H3h-C2c
 * sacrificial range, starting at logical block 2048.
 */
#define TR2_SDMMC2_E1_TEST_BLOCK UINT32_C(2048)
#define TR2_SDMMC2_E1_TEST_BLOCK_COUNT UINT32_C(4)
#define TR2_SDMMC2_E1_TEST_SIZE (TR2_SDMMC2_E1_TEST_BLOCK_COUNT * 512U)
#define TR2_SDMMC2_E1_PARTIAL_OFFSET UINT32_C(500)
#define TR2_SDMMC2_E1_PARTIAL_SIZE UINT32_C(37)
#define TR2_SDMMC2_E1_ALIGNED_OFFSET UINT32_C(1024)
#define TR2_SDMMC2_E1_ALIGNED_SIZE UINT32_C(1024)
#define TR2_SDMMC2_WRITE_TIMEOUT_MS 2000U
#define TR2_SDMMC2_READY_TIMEOUT_MS 2000U

static uint8_t tr2_sdmmc2_e1_sector_scratch[512U];
static uint8_t tr2_sdmmc2_e1_expected[TR2_SDMMC2_E1_TEST_SIZE];
static uint8_t tr2_sdmmc2_e1_verify[TR2_SDMMC2_E1_TEST_SIZE];
static uint8_t tr2_sdmmc2_e1_partial[TR2_SDMMC2_E1_PARTIAL_SIZE];
static uint8_t tr2_sdmmc2_e1_aligned[TR2_SDMMC2_E1_ALIGNED_SIZE];

volatile uint32_t tr2_sdmmc2_e1_init_result = UINT32_MAX;
volatile uint32_t tr2_sdmmc2_e1_capacity_result = UINT32_MAX;
volatile uint64_t tr2_sdmmc2_e1_capacity_bytes = 0U;
volatile uint32_t tr2_sdmmc2_e1_seed_write_result = UINT32_MAX;
volatile uint32_t tr2_sdmmc2_e1_partial_write_result = UINT32_MAX;
volatile uint32_t tr2_sdmmc2_e1_aligned_write_result = UINT32_MAX;
volatile uint32_t tr2_sdmmc2_e1_sync_result = UINT32_MAX;
volatile uint32_t tr2_sdmmc2_e1_read_result = UINT32_MAX;
volatile uint32_t tr2_sdmmc2_e1_mismatch_count = 0U;
volatile uint32_t tr2_sdmmc2_e1_first_mismatch = UINT32_MAX;


/*
 * H3h-E3 physical end-to-end qualification of D5-C over E1.
 *
 * D5-C expects metadata at logical offset 0.  Never expose the whole card to
 * this destructive test: this bounded view maps logical offset 0 to physical
 * block 2048 and limits every operation to the already-authorized H3h-C2c
 * sacrificial range 2048..3071 inclusive.
 */
#define TR2_SDMMC2_E3_WINDOW_BLOCK UINT32_C(2048)
#define TR2_SDMMC2_E3_WINDOW_BLOCK_COUNT UINT32_C(1024)
#define TR2_SDMMC2_E3_WINDOW_SIZE \
    ((uint64_t)TR2_SDMMC2_E3_WINDOW_BLOCK_COUNT * UINT64_C(512))
#define TR2_SDMMC2_E3_CAMPAIGN_ID UINT32_C(0xE301)
#define TR2_SDMMC2_E3_CHECKPOINT_BYTES ((size_t)80U)
#define TR2_SDMMC2_E3_TAIL_BYTES ((size_t)64U)
#define TR2_SDMMC2_E3_PAYLOAD_BUFFER_SIZE ((size_t)64U)
#define TR2_SDMMC2_E3_BLOCK_SCRATCH_SIZE ((size_t)128U)

typedef struct {
    CampaignBulkMedia *underlying;
    uint64_t base;
    uint64_t size;
} Tr2BulkMediaWindow;

static uint8_t tr2_sdmmc2_e3_payload_buffer_a[TR2_SDMMC2_E3_PAYLOAD_BUFFER_SIZE];
static uint8_t tr2_sdmmc2_e3_payload_buffer_b[TR2_SDMMC2_E3_PAYLOAD_BUFFER_SIZE];
static uint8_t tr2_sdmmc2_e3_block_scratch_a[TR2_SDMMC2_E3_BLOCK_SCRATCH_SIZE];
static uint8_t tr2_sdmmc2_e3_block_scratch_b[TR2_SDMMC2_E3_BLOCK_SCRATCH_SIZE];
static uint8_t tr2_sdmmc2_e3_checkpoint_data[TR2_SDMMC2_E3_CHECKPOINT_BYTES];
static uint8_t tr2_sdmmc2_e3_tail_data[TR2_SDMMC2_E3_TAIL_BYTES];
static uint8_t tr2_sdmmc2_e3_metadata_clear[TR2_CAMPAIGN_BULK_METADATA_BYTES];

volatile uint32_t tr2_sdmmc2_e3_window_clear_result = UINT32_MAX;
volatile uint32_t tr2_sdmmc2_e3_window_clear_sync_result = UINT32_MAX;
volatile uint32_t tr2_sdmmc2_e3_store_init_result = UINT32_MAX;
volatile uint32_t tr2_sdmmc2_e3_begin_result = UINT32_MAX;
volatile uint32_t tr2_sdmmc2_e3_append_checkpoint_result = UINT32_MAX;
volatile uint32_t tr2_sdmmc2_e3_checkpoint_result = UINT32_MAX;
volatile uint64_t tr2_sdmmc2_e3_post_checkpoint_offset = 0U;
volatile uint32_t tr2_sdmmc2_e3_append_tail_result = UINT32_MAX;
volatile uint64_t tr2_sdmmc2_e3_post_tail_offset = 0U;
volatile uint32_t tr2_sdmmc2_e3_reboot_store_init_result = UINT32_MAX;
volatile uint32_t tr2_sdmmc2_e3_recover_result = UINT32_MAX;
volatile uint32_t tr2_sdmmc2_e3_recovery_status = UINT32_MAX;
volatile uint64_t tr2_sdmmc2_e3_recovered_prefix_bytes = 0U;

static bool BulkWindowRangeFits(const Tr2BulkMediaWindow *window,
                                uint64_t offset,
                                size_t size)
{
    return window != NULL && offset <= window->size &&
           (uint64_t)size <= window->size - offset;
}

static Tr2Result BulkWindowCapacity(void *context, uint64_t *capacity)
{
    Tr2BulkMediaWindow *window = (Tr2BulkMediaWindow *)context;

    if (window == NULL || capacity == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    *capacity = window->size;
    return TR2_OK;
}

static Tr2Result BulkWindowRead(void *context,
                                uint64_t offset,
                                void *buffer,
                                size_t size)
{
    Tr2BulkMediaWindow *window = (Tr2BulkMediaWindow *)context;

    if (window == NULL || window->underlying == NULL ||
        !BulkWindowRangeFits(window, offset, size) ||
        offset > UINT64_MAX - window->base) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    return campaign_bulk_media_read(window->underlying,
                                    window->base + offset,
                                    buffer,
                                    size);
}

static Tr2Result BulkWindowWrite(void *context,
                                 uint64_t offset,
                                 const void *buffer,
                                 size_t size)
{
    Tr2BulkMediaWindow *window = (Tr2BulkMediaWindow *)context;

    if (window == NULL || window->underlying == NULL ||
        !BulkWindowRangeFits(window, offset, size) ||
        offset > UINT64_MAX - window->base) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    return campaign_bulk_media_write(window->underlying,
                                     window->base + offset,
                                     buffer,
                                     size);
}

static Tr2Result BulkWindowSync(void *context)
{
    Tr2BulkMediaWindow *window = (Tr2BulkMediaWindow *)context;

    if (window == NULL || window->underlying == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    return campaign_bulk_media_sync(window->underlying);
}

static CampaignBulkMedia BulkWindowInterface(Tr2BulkMediaWindow *window)
{
    CampaignBulkMedia media = {
        window, BulkWindowCapacity, BulkWindowRead, BulkWindowWrite,
        BulkWindowSync
    };
    return media;
}


/*
 * H3h-E4 physical power-loss qualification.
 *
 * One binary drives both cut scenarios from durable on-card state.  The
 * fault-injection media delegates real I/O, then latches at deterministic
 * transaction boundaries.  The operator removes board power only after the
 * cut flag/stage is visible.
 */
#define TR2_SDMMC2_E4_WINDOW_BLOCK UINT32_C(4096)
#define TR2_SDMMC2_E4_WINDOW_BLOCK_COUNT UINT32_C(1024)
#define TR2_SDMMC2_E4_WINDOW_SIZE \
    ((uint64_t)TR2_SDMMC2_E4_WINDOW_BLOCK_COUNT * UINT64_C(512))
#define TR2_SDMMC2_E4_MARKER_BLOCK UINT32_C(5120)
#define TR2_SDMMC2_E4_MARKER_OFFSET \
    ((uint64_t)TR2_SDMMC2_E4_MARKER_BLOCK * UINT64_C(512))
#define TR2_SDMMC2_E4_MARKER_MAGIC UINT32_C(0x45344632)
#define TR2_SDMMC2_E4_MARKER_STATE_FRESH UINT32_C(1)
#define TR2_SDMMC2_E4_MARKER_STATE_PAYLOAD_CUT UINT32_C(2)
#define TR2_SDMMC2_E4_MARKER_STATE_METADATA_CUT UINT32_C(3)
#define TR2_SDMMC2_E4_MARKER_STATE_DONE UINT32_C(4)
#define TR2_SDMMC2_E4_CAMPAIGN_PAYLOAD_CUT UINT32_C(0xE411)
#define TR2_SDMMC2_E4_CAMPAIGN_METADATA_CUT UINT32_C(0xE412)
#define TR2_SDMMC2_E4_RECORD_BYTES ((size_t)16U)

typedef enum {
    TR2_E4_INJECT_NONE = 0,
    TR2_E4_INJECT_AFTER_PAYLOAD_SYNC,
    TR2_E4_INJECT_AFTER_METADATA_WRITE
} Tr2E4InjectMode;

typedef struct {
    CampaignBulkMedia underlying;
    Tr2E4InjectMode mode;
    uint64_t metadata_limit;
} Tr2E4Media;

static uint8_t tr2_sdmmc2_e4_payload_buffer[TR2_SDMMC2_E3_PAYLOAD_BUFFER_SIZE];
static uint8_t tr2_sdmmc2_e4_block_scratch[TR2_SDMMC2_E3_BLOCK_SCRATCH_SIZE];
static uint8_t tr2_sdmmc2_e4_record[TR2_SDMMC2_E4_RECORD_BYTES];

typedef struct {
    uint32_t magic;
    uint32_t state;
    uint32_t state_inverse;
} Tr2E4Marker;

static Tr2Result E4MarkerRead(CampaignBulkMedia *media, Tr2E4Marker *marker)
{
    Tr2E4Marker raw;
    Tr2Result result;

    if (media == NULL || marker == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    result = campaign_bulk_media_read(media,
                                      TR2_SDMMC2_E4_MARKER_OFFSET,
                                      &raw,
                                      sizeof(raw));
    if (result != TR2_OK) {
        return result;
    }
    if (raw.magic != TR2_SDMMC2_E4_MARKER_MAGIC ||
        raw.state_inverse != ~raw.state ||
        (raw.state != TR2_SDMMC2_E4_MARKER_STATE_FRESH &&
         raw.state != TR2_SDMMC2_E4_MARKER_STATE_PAYLOAD_CUT &&
         raw.state != TR2_SDMMC2_E4_MARKER_STATE_METADATA_CUT &&
         raw.state != TR2_SDMMC2_E4_MARKER_STATE_DONE)) {
        marker->magic = 0U;
        marker->state = 0U;
        marker->state_inverse = 0U;
        return TR2_OK;
    }
    *marker = raw;
    return TR2_OK;
}

static Tr2Result E4MarkerWrite(CampaignBulkMedia *media, uint32_t state)
{
    Tr2E4Marker raw = {
        TR2_SDMMC2_E4_MARKER_MAGIC,
        state,
        ~state
    };
    Tr2Result result = campaign_bulk_media_write(media,
                                                 TR2_SDMMC2_E4_MARKER_OFFSET,
                                                 &raw,
                                                 sizeof(raw));
    if (result != TR2_OK) {
        return result;
    }
    return campaign_bulk_media_sync(media);
}

static Tr2Result E4PrepareFresh(CampaignBulkMedia *media)
{
    Tr2Result result;

    memset(tr2_sdmmc2_e3_metadata_clear, 0, sizeof(tr2_sdmmc2_e3_metadata_clear));
    result = campaign_bulk_media_write(media,
                                       (uint64_t)TR2_SDMMC2_E4_WINDOW_BLOCK *
                                           UINT64_C(512),
                                       tr2_sdmmc2_e3_metadata_clear,
                                       sizeof(tr2_sdmmc2_e3_metadata_clear));
    if (result != TR2_OK) {
        return result;
    }
    result = campaign_bulk_media_sync(media);
    if (result != TR2_OK) {
        return result;
    }
    return E4MarkerWrite(media, TR2_SDMMC2_E4_MARKER_STATE_FRESH);
}

volatile uint32_t tr2_sdmmc2_e4_phase = 0U;
volatile uint32_t tr2_sdmmc2_e4_cut_point_reached = 0U;
volatile uint32_t tr2_sdmmc2_e4_last_result = UINT32_MAX;
volatile uint32_t tr2_sdmmc2_e4_recovery_status = UINT32_MAX;
volatile uint64_t tr2_sdmmc2_e4_recovered_prefix_bytes = 0U;

static void E4CutPowerPoint(uint32_t phase)
{
    tr2_sdmmc2_e4_phase = phase;
    tr2_sdmmc2_e4_cut_point_reached = 1U;
    HAL_GPIO_WritePin(TR2_BRINGUP_LED_PORT,
                      TR2_BRINGUP_LED_PIN,
                      GPIO_PIN_SET);
    __disable_irq();
    for (;;) {
        /* Deterministic physical cut point: operator removes board power. */
    }
}

static Tr2Result E4Capacity(void *context, uint64_t *capacity)
{
    Tr2E4Media *adapter = (Tr2E4Media *)context;
    return campaign_bulk_media_capacity(&adapter->underlying, capacity);
}

static Tr2Result E4Read(void *context,
                        uint64_t offset,
                        void *buffer,
                        size_t size)
{
    Tr2E4Media *adapter = (Tr2E4Media *)context;
    return campaign_bulk_media_read(&adapter->underlying, offset, buffer, size);
}

static Tr2Result E4Write(void *context,
                         uint64_t offset,
                         const void *buffer,
                         size_t size)
{
    Tr2E4Media *adapter = (Tr2E4Media *)context;
    Tr2Result result =
        campaign_bulk_media_write(&adapter->underlying, offset, buffer, size);

    if (result == TR2_OK &&
        adapter->mode == TR2_E4_INJECT_AFTER_METADATA_WRITE &&
        offset < adapter->metadata_limit &&
        size == TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE) {
        E4CutPowerPoint(4U);
    }
    return result;
}

static Tr2Result E4Sync(void *context)
{
    Tr2E4Media *adapter = (Tr2E4Media *)context;
    Tr2Result result = campaign_bulk_media_sync(&adapter->underlying);

    if (result != TR2_OK) {
        return result;
    }
    /*
     * The injection mode is armed immediately before the transaction under
     * test. Do not infer the transaction from a global sync counter: earlier
     * begin/checkpoint operations may already have synchronized the medium.
     */
    if (adapter->mode == TR2_E4_INJECT_AFTER_PAYLOAD_SYNC) {
        E4CutPowerPoint(2U);
    }
    return TR2_OK;
}

static CampaignBulkMedia E4Interface(Tr2E4Media *adapter)
{
    CampaignBulkMedia media = {
        adapter, E4Capacity, E4Read, E4Write, E4Sync
    };
    return media;
}

#define TR2_IIS3DWB_CS_PORT GPIOC
#define TR2_IIS3DWB_CS_PIN GPIO_PIN_9
#define TR2_IIS3DWB_WHO_AM_I_REG 0x0FU
#define TR2_IIS3DWB_WHO_AM_I_EXPECTED 0x7BU
#define TR2_IIS3DWB_SPI_TIMEOUT_MS 10U
#define TR2_IIS3DWB_CTRL1_XL_REG 0x10U
#define TR2_IIS3DWB_CTRL3_C_REG 0x12U
#define TR2_IIS3DWB_STATUS_REG 0x1EU
#define TR2_IIS3DWB_OUTX_L_A_REG 0x28U
#define TR2_IIS3DWB_CTRL1_XL_2G_26K7HZ 0xA0U
#define TR2_IIS3DWB_CTRL3_C_BDU_IF_INC 0x44U
#define TR2_IIS3DWB_STATUS_XLDA 0x01U

volatile uint32_t tr2_iis3dwb_spi_init_ok = 0U;
volatile uint32_t tr2_iis3dwb_whoami_status = (uint32_t)HAL_ERROR;
volatile uint8_t tr2_iis3dwb_whoami = 0U;
volatile uint8_t tr2_iis3dwb_whoami_matches = 0U;
volatile uint32_t tr2_iis3dwb_config_status = (uint32_t)HAL_ERROR;
volatile uint32_t tr2_iis3dwb_sample_status = (uint32_t)HAL_ERROR;
volatile uint8_t tr2_iis3dwb_data_ready = 0U;
volatile int16_t tr2_iis3dwb_raw_x = 0;
volatile int16_t tr2_iis3dwb_raw_y = 0;
volatile int16_t tr2_iis3dwb_raw_z = 0;

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
#define TR2_FRAM_D2D4_ALLOW_PUBLISHED_CANDIDATE 0U
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

/*
 * D3-B bounded physical timing qualification.
 *
 * Armed only for the explicitly observed D2-D final authority gen4/B/0xA6.
 * Raw write timings target the inactive image-A payload, which is not
 * authoritative.  The full-size raw write is performed before the commit;
 * the subsequent normal H3d2 commit overwrites that same inactive image and
 * publishes generation 5 atomically.  No format, retry or repair is used.
 *
 * HAL_GetTick() provides millisecond elapsed times.  These are descriptive
 * qualification measurements, not production performance requirements.
 */
#define TR2_FRAM_D3B_ALLOW_TIMING 0U
#define TR2_FRAM_D3B_TEST_VALUE UINT8_C(0xA9)

volatile uint32_t tr2_fram_d3b_attempted = 0U;
volatile uint32_t tr2_fram_d3b_completed = 0U;
volatile uint32_t tr2_fram_d3b_read64_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d3b_read64_ms = 0U;
volatile uint32_t tr2_fram_d3b_write64_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d3b_write64_ms = 0U;
volatile uint32_t tr2_fram_d3b_read_full_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d3b_read_full_ms = 0U;
volatile uint32_t tr2_fram_d3b_write_full_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d3b_write_full_ms = 0U;
volatile uint32_t tr2_fram_d3b_commit_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d3b_commit_ms = 0U;
volatile uint32_t tr2_fram_d3b_post_recover_result = (uint32_t)TR2_ERROR_INTERNAL;
volatile uint32_t tr2_fram_d3b_post_status = (uint32_t)TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;
volatile uint64_t tr2_fram_d3b_post_generation = UINT64_C(0);
volatile uint8_t tr2_fram_d3b_post_active_image = 0xFFU;
volatile uint8_t tr2_fram_d3b_post_value = 0U;

/* D3-C bounded normal-commit alternation qualification. */
#define TR2_FRAM_D3C_ALLOW_ALTERNATION 0U
#define TR2_FRAM_D3C_COMMIT_COUNT 3U

volatile uint32_t tr2_fram_d3c_attempted = 0U;
volatile uint32_t tr2_fram_d3c_completed = 0U;
volatile uint32_t tr2_fram_d3c_commit_result[TR2_FRAM_D3C_COMMIT_COUNT] = {
    (uint32_t)TR2_ERROR_INTERNAL, (uint32_t)TR2_ERROR_INTERNAL,
    (uint32_t)TR2_ERROR_INTERNAL};
volatile uint32_t tr2_fram_d3c_recover_result[TR2_FRAM_D3C_COMMIT_COUNT] = {
    (uint32_t)TR2_ERROR_INTERNAL, (uint32_t)TR2_ERROR_INTERNAL,
    (uint32_t)TR2_ERROR_INTERNAL};
volatile uint64_t tr2_fram_d3c_generation[TR2_FRAM_D3C_COMMIT_COUNT] = {0U, 0U, 0U};
volatile uint8_t tr2_fram_d3c_active_image[TR2_FRAM_D3C_COMMIT_COUNT] = {0xFFU, 0xFFU, 0xFFU};
volatile uint8_t tr2_fram_d3c_active_superblock[TR2_FRAM_D3C_COMMIT_COUNT] = {0xFFU, 0xFFU, 0xFFU};
volatile uint8_t tr2_fram_d3c_value[TR2_FRAM_D3C_COMMIT_COUNT] = {0U, 0U, 0U};

static void SystemClock_Config(void);
static void SystemPower_Config(void);
static void BringupLed_Init(void);
static void FramSpi_Init(void);
static void Iis3dwbSpi_Init(void);
static void Sdmmc2_Bringup(void);
static HAL_StatusTypeDef Fram_ReadDeviceId(uint8_t device_id[TR2_FRAM_RDID_SIZE]);
static void Sdmmc2_Bringup(void)
{
    HAL_SD_CardInfoTypeDef card_info = {0};
    GPIO_InitTypeDef gpio = {0};
    RCC_PeriphCLKInitTypeDef periph = {0};
    Stm32SdmmcBulkMedia adapter;
    CampaignBulkMedia media;
    const uint64_t test_offset =
        (uint64_t)TR2_SDMMC2_E1_TEST_BLOCK * UINT64_C(512);

    tr2_sdmmc2_init_attempted = 1U;
    tr2_sdmmc2_stage = 1U;

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    gpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    gpio.Alternate = GPIO_AF11_SDMMC2;
    HAL_GPIO_Init(GPIOD, &gpio);

    gpio.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_14 | GPIO_PIN_15;
    gpio.Alternate = GPIO_AF12_SDMMC2;
    HAL_GPIO_Init(GPIOB, &gpio);

    periph.PeriphClockSelection = RCC_PERIPHCLK_SDMMC;
    periph.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL1;
    if (HAL_RCCEx_PeriphCLKConfig(&periph) != HAL_OK) {
        tr2_sdmmc2_stage = 2U;
        tr2_sdmmc2_init_status = (uint32_t)HAL_ERROR;
        return;
    }

    tr2_sdmmc2_stage = 3U;
    __HAL_RCC_SDMMC2_CLK_ENABLE();
    tr2_sdmmc2_kernel_clock_hz =
        HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC);

    hsd2.Instance = SDMMC2;
    hsd2.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
    hsd2.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
    hsd2.Init.BusWide = SDMMC_BUS_WIDE_1B;
    hsd2.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd2.Init.ClockDiv = 8U;

    tr2_sdmmc2_stage = 4U;
    tr2_sdmmc2_init_status = (uint32_t)HAL_SD_Init(&hsd2);
    tr2_sdmmc2_error_code = hsd2.ErrorCode;
    if (tr2_sdmmc2_init_status != (uint32_t)HAL_OK) {
        tr2_sdmmc2_last_cmd = SDMMC2->CMD & SDMMC_CMD_CMDINDEX;
        tr2_sdmmc2_pre_cmd16_card_type = hsd2.SdCard.CardType;
        tr2_sdmmc2_pre_cmd16_card_version = hsd2.SdCard.CardVersion;
        tr2_sdmmc2_pre_cmd16_card_class = hsd2.SdCard.Class;
        tr2_sdmmc2_pre_cmd16_rca = hsd2.SdCard.RelCardAdd;
        tr2_sdmmc2_pre_cmd16_block_nbr = hsd2.SdCard.BlockNbr;
        tr2_sdmmc2_pre_cmd16_block_size = hsd2.SdCard.BlockSize;
        tr2_sdmmc2_pre_cmd16_log_block_nbr = hsd2.SdCard.LogBlockNbr;
        tr2_sdmmc2_pre_cmd16_log_block_size = hsd2.SdCard.LogBlockSize;
        tr2_sdmmc2_sta = SDMMC2->STA;
        tr2_sdmmc2_resp1 = SDMMC2->RESP1;
        tr2_sdmmc2_clkcr = SDMMC2->CLKCR;
        tr2_sdmmc2_power = SDMMC2->POWER;
        tr2_sdmmc2_stage = 5U;
        return;
    }

    tr2_sdmmc2_stage = 6U;
    tr2_sdmmc2_card_info_status =
        (uint32_t)HAL_SD_GetCardInfo(&hsd2, &card_info);
    tr2_sdmmc2_error_code = hsd2.ErrorCode;
    if (tr2_sdmmc2_card_info_status != (uint32_t)HAL_OK) {
        tr2_sdmmc2_stage = 7U;
        return;
    }

    tr2_sdmmc2_card_type = card_info.CardType;
    tr2_sdmmc2_card_version = card_info.CardVersion;
    tr2_sdmmc2_card_class = card_info.Class;
    tr2_sdmmc2_relative_card_address = card_info.RelCardAdd;
    tr2_sdmmc2_log_block_nbr = card_info.LogBlockNbr;
    tr2_sdmmc2_log_block_size = card_info.LogBlockSize;
    tr2_sdmmc2_block_nbr = card_info.BlockNbr;
    tr2_sdmmc2_block_size = card_info.BlockSize;

    if ((card_info.LogBlockSize != 512U) ||
        (card_info.LogBlockNbr <= TR2_SDMMC2_E4_MARKER_BLOCK)) {
        tr2_sdmmc2_stage = 8U;
        return;
    }

    tr2_sdmmc2_e1_init_result =
        (uint32_t)stm32_sdmmc_bulk_media_init(
            &adapter,
            &hsd2,
            tr2_sdmmc2_e1_sector_scratch,
            sizeof(tr2_sdmmc2_e1_sector_scratch),
            TR2_SDMMC2_WRITE_TIMEOUT_MS,
            TR2_SDMMC2_READY_TIMEOUT_MS);
    if (tr2_sdmmc2_e1_init_result != (uint32_t)TR2_OK) {
        tr2_sdmmc2_stage = 9U;
        return;
    }

    media = stm32_sdmmc_bulk_media_interface(&adapter);
    {
        uint64_t capacity_bytes = 0U;

        tr2_sdmmc2_e1_capacity_result =
            (uint32_t)campaign_bulk_media_capacity(&media, &capacity_bytes);
        tr2_sdmmc2_e1_capacity_bytes = capacity_bytes;
    }
    if (tr2_sdmmc2_e1_capacity_result != (uint32_t)TR2_OK) {
        tr2_sdmmc2_stage = 10U;
        return;
    }

    for (uint32_t i = 0U; i < TR2_SDMMC2_E1_TEST_SIZE; ++i) {
        tr2_sdmmc2_e1_expected[i] =
            (uint8_t)(((i * UINT32_C(29)) + UINT32_C(0x31)) & UINT32_C(0xFF));
    }
    for (uint32_t i = 0U; i < TR2_SDMMC2_E1_PARTIAL_SIZE; ++i) {
        tr2_sdmmc2_e1_partial[i] =
            (uint8_t)(((i * UINT32_C(17)) + UINT32_C(0xA3)) & UINT32_C(0xFF));
        tr2_sdmmc2_e1_expected[TR2_SDMMC2_E1_PARTIAL_OFFSET + i] =
            tr2_sdmmc2_e1_partial[i];
    }
    for (uint32_t i = 0U; i < TR2_SDMMC2_E1_ALIGNED_SIZE; ++i) {
        tr2_sdmmc2_e1_aligned[i] =
            (uint8_t)(((i * UINT32_C(43)) + UINT32_C(0x5C)) & UINT32_C(0xFF));
        tr2_sdmmc2_e1_expected[TR2_SDMMC2_E1_ALIGNED_OFFSET + i] =
            tr2_sdmmc2_e1_aligned[i];
    }

    tr2_sdmmc2_e1_seed_write_result =
        (uint32_t)campaign_bulk_media_write(
            &media,
            test_offset,
            tr2_sdmmc2_e1_expected,
            TR2_SDMMC2_E1_TEST_SIZE);
    if (tr2_sdmmc2_e1_seed_write_result != (uint32_t)TR2_OK) {
        tr2_sdmmc2_stage = 11U;
        return;
    }

    /*
     * Restore the seed bytes in RAM before applying the two modifications:
     * the first write above intentionally established the baseline media image.
     */
    for (uint32_t i = 0U; i < TR2_SDMMC2_E1_TEST_SIZE; ++i) {
        tr2_sdmmc2_e1_expected[i] =
            (uint8_t)(((i * UINT32_C(29)) + UINT32_C(0x31)) & UINT32_C(0xFF));
    }

    tr2_sdmmc2_e1_partial_write_result =
        (uint32_t)campaign_bulk_media_write(
            &media,
            test_offset + TR2_SDMMC2_E1_PARTIAL_OFFSET,
            tr2_sdmmc2_e1_partial,
            TR2_SDMMC2_E1_PARTIAL_SIZE);
    if (tr2_sdmmc2_e1_partial_write_result != (uint32_t)TR2_OK) {
        tr2_sdmmc2_stage = 12U;
        return;
    }
    for (uint32_t i = 0U; i < TR2_SDMMC2_E1_PARTIAL_SIZE; ++i) {
        tr2_sdmmc2_e1_expected[TR2_SDMMC2_E1_PARTIAL_OFFSET + i] =
            tr2_sdmmc2_e1_partial[i];
    }

    tr2_sdmmc2_e1_aligned_write_result =
        (uint32_t)campaign_bulk_media_write(
            &media,
            test_offset + TR2_SDMMC2_E1_ALIGNED_OFFSET,
            tr2_sdmmc2_e1_aligned,
            TR2_SDMMC2_E1_ALIGNED_SIZE);
    if (tr2_sdmmc2_e1_aligned_write_result != (uint32_t)TR2_OK) {
        tr2_sdmmc2_stage = 13U;
        return;
    }
    for (uint32_t i = 0U; i < TR2_SDMMC2_E1_ALIGNED_SIZE; ++i) {
        tr2_sdmmc2_e1_expected[TR2_SDMMC2_E1_ALIGNED_OFFSET + i] =
            tr2_sdmmc2_e1_aligned[i];
    }

    tr2_sdmmc2_e1_sync_result =
        (uint32_t)campaign_bulk_media_sync(&media);
    if (tr2_sdmmc2_e1_sync_result != (uint32_t)TR2_OK) {
        tr2_sdmmc2_stage = 14U;
        return;
    }

    memset(tr2_sdmmc2_e1_verify, 0, sizeof(tr2_sdmmc2_e1_verify));
    tr2_sdmmc2_e1_read_result =
        (uint32_t)campaign_bulk_media_read(
            &media,
            test_offset,
            tr2_sdmmc2_e1_verify,
            TR2_SDMMC2_E1_TEST_SIZE);
    if (tr2_sdmmc2_e1_read_result != (uint32_t)TR2_OK) {
        tr2_sdmmc2_stage = 15U;
        return;
    }

    for (uint32_t i = 0U; i < TR2_SDMMC2_E1_TEST_SIZE; ++i) {
        if (tr2_sdmmc2_e1_verify[i] != tr2_sdmmc2_e1_expected[i]) {
            if (tr2_sdmmc2_e1_mismatch_count == 0U) {
                tr2_sdmmc2_e1_first_mismatch = i;
            }
            ++tr2_sdmmc2_e1_mismatch_count;
        }
    }

    tr2_sdmmc2_error_code = hsd2.ErrorCode;
    if (tr2_sdmmc2_e1_mismatch_count != 0U) {
        tr2_sdmmc2_stage = 17U;
        return;
    }

    /*
     * E4 runs before the historical E3 probe.  Every non-final E4 path
     * latches or returns, so E3 cannot clear/modify the qualification state.
     */
    {
        Tr2BulkMediaWindow window = {
            &media,
            (uint64_t)TR2_SDMMC2_E4_WINDOW_BLOCK * UINT64_C(512),
            TR2_SDMMC2_E4_WINDOW_SIZE
        };
        CampaignBulkMedia bounded = BulkWindowInterface(&window);
        CampaignDataStoreBulk recovery_store;
        CampaignDataStore *iface;
        Tr2E4Marker marker;
        CampaignDataRecoveryResult r401;
        CampaignDataRecoveryResult r402;
        Tr2Result result;

        result = E4MarkerRead(&media, &marker);
        if (result != TR2_OK) {
            tr2_sdmmc2_e4_last_result = (uint32_t)result;
            tr2_sdmmc2_stage = 29U;
            return;
        }
        if (marker.state == 0U) {
            result = E4PrepareFresh(&media);
            if (result != TR2_OK) {
                tr2_sdmmc2_e4_last_result = (uint32_t)result;
                tr2_sdmmc2_stage = 29U;
                return;
            }
            marker.state = TR2_SDMMC2_E4_MARKER_STATE_FRESH;
        }

        result = campaign_data_store_bulk_init(
            &recovery_store,
            &bounded,
            tr2_sdmmc2_e4_payload_buffer,
            sizeof(tr2_sdmmc2_e4_payload_buffer),
            tr2_sdmmc2_e4_block_scratch,
            sizeof(tr2_sdmmc2_e4_block_scratch));
        tr2_sdmmc2_e4_last_result = (uint32_t)result;
        if (result != TR2_OK) {
            tr2_sdmmc2_stage = 29U;
            return;
        }
        iface = campaign_data_store_bulk_interface(&recovery_store);

        result = iface->recover_campaign(iface->context,
                                         TR2_SDMMC2_E4_CAMPAIGN_PAYLOAD_CUT,
                                         &r401);
        tr2_sdmmc2_e4_last_result = (uint32_t)result;
        if (result != TR2_OK) {
            tr2_sdmmc2_stage = 30U;
            return;
        }
        result = iface->recover_campaign(iface->context,
                                         TR2_SDMMC2_E4_CAMPAIGN_METADATA_CUT,
                                         &r402);
        tr2_sdmmc2_e4_last_result = (uint32_t)result;
        if (result != TR2_OK) {
            tr2_sdmmc2_stage = 31U;
            return;
        }

        for (size_t i = 0U; i < sizeof(tr2_sdmmc2_e4_record); ++i) {
            tr2_sdmmc2_e4_record[i] =
                (uint8_t)(((i * (size_t)23U) + (size_t)0x41U) &
                          (size_t)0xFFU);
        }

        /* A partially prepared campaign is not proof of corruption or a cut. */
        if (marker.state == TR2_SDMMC2_E4_MARKER_STATE_FRESH &&
            (r401.status != CAMPAIGN_DATA_RECOVERY_EMPTY ||
             r402.status != CAMPAIGN_DATA_RECOVERY_EMPTY)) {
            const CampaignDataRecoveryResult *interrupted =
                r401.status != CAMPAIGN_DATA_RECOVERY_EMPTY ? &r401 : &r402;
            tr2_sdmmc2_e4_recovery_status = (uint32_t)interrupted->status;
            tr2_sdmmc2_e4_recovered_prefix_bytes =
                interrupted->durable_prefix_bytes;
            tr2_sdmmc2_stage = 34U;
            return;
        }

        if (marker.state == TR2_SDMMC2_E4_MARKER_STATE_FRESH &&
            r401.status == CAMPAIGN_DATA_RECOVERY_EMPTY) {
            CampaignDataStoreBulk active_store;
            Tr2E4Media injector = {
                bounded, TR2_E4_INJECT_NONE,
                TR2_CAMPAIGN_BULK_METADATA_BYTES
            };
            CampaignBulkMedia injected = E4Interface(&injector);

            tr2_sdmmc2_e4_phase = 1U;
            result = campaign_data_store_bulk_init(
                &active_store,
                &injected,
                tr2_sdmmc2_e4_payload_buffer,
                sizeof(tr2_sdmmc2_e4_payload_buffer),
                tr2_sdmmc2_e4_block_scratch,
                sizeof(tr2_sdmmc2_e4_block_scratch));
            if (result == TR2_OK) {
                iface = campaign_data_store_bulk_interface(&active_store);
                result = iface->begin_campaign(
                    iface->context, TR2_SDMMC2_E4_CAMPAIGN_PAYLOAD_CUT);
            }
            if (result == TR2_OK) {
                result = iface->append(
                    iface->context,
                    TR2_SDMMC2_E4_CAMPAIGN_PAYLOAD_CUT,
                    tr2_sdmmc2_e4_record,
                    sizeof(tr2_sdmmc2_e4_record));
            }
            if (result == TR2_OK) {
                result = iface->checkpoint(
                    iface->context, TR2_SDMMC2_E4_CAMPAIGN_PAYLOAD_CUT);
            }
            if (result == TR2_OK) {
                result = iface->append(
                    iface->context,
                    TR2_SDMMC2_E4_CAMPAIGN_PAYLOAD_CUT,
                    tr2_sdmmc2_e4_record,
                    sizeof(tr2_sdmmc2_e4_record));
            }
            tr2_sdmmc2_e4_last_result = (uint32_t)result;
            if (result != TR2_OK) {
                tr2_sdmmc2_stage = 32U;
                return;
            }

            result = E4MarkerWrite(
                &media, TR2_SDMMC2_E4_MARKER_STATE_PAYLOAD_CUT);
            if (result != TR2_OK) {
                tr2_sdmmc2_e4_last_result = (uint32_t)result;
                tr2_sdmmc2_stage = 33U;
                return;
            }
            injector.mode = TR2_E4_INJECT_AFTER_PAYLOAD_SYNC;
            result = iface->checkpoint(
                iface->context, TR2_SDMMC2_E4_CAMPAIGN_PAYLOAD_CUT);
            tr2_sdmmc2_e4_last_result = (uint32_t)result;
            tr2_sdmmc2_stage = 33U;
            return;
        }

        /* Every later state must preserve the payload-cut authority. */
        if (r401.status != CAMPAIGN_DATA_RECOVERY_VALID ||
            r401.durable_prefix_bytes != UINT64_C(16)) {
            tr2_sdmmc2_e4_recovery_status = (uint32_t)r401.status;
            tr2_sdmmc2_e4_recovered_prefix_bytes =
                r401.durable_prefix_bytes;
            tr2_sdmmc2_stage =
                marker.state == TR2_SDMMC2_E4_MARKER_STATE_PAYLOAD_CUT
                    ? 34U : 37U;
            return;
        }

        if (marker.state == TR2_SDMMC2_E4_MARKER_STATE_PAYLOAD_CUT &&
            r402.status != CAMPAIGN_DATA_RECOVERY_EMPTY) {
            /* Metadata preparation may have been interrupted before its marker. */
            tr2_sdmmc2_e4_recovery_status = (uint32_t)r402.status;
            tr2_sdmmc2_e4_recovered_prefix_bytes =
                r402.durable_prefix_bytes;
            tr2_sdmmc2_stage = 37U;
            return;
        }

        if (marker.state == TR2_SDMMC2_E4_MARKER_STATE_PAYLOAD_CUT &&
            r402.status == CAMPAIGN_DATA_RECOVERY_EMPTY) {
            CampaignDataStoreBulk active_store;
            Tr2E4Media injector = {
                bounded, TR2_E4_INJECT_NONE,
                TR2_CAMPAIGN_BULK_METADATA_BYTES
            };
            CampaignBulkMedia injected = E4Interface(&injector);

            tr2_sdmmc2_e4_phase = 3U;
            tr2_sdmmc2_e4_recovery_status = (uint32_t)r401.status;
            tr2_sdmmc2_e4_recovered_prefix_bytes =
                r401.durable_prefix_bytes;
            result = campaign_data_store_bulk_init(
                &active_store,
                &injected,
                tr2_sdmmc2_e4_payload_buffer,
                sizeof(tr2_sdmmc2_e4_payload_buffer),
                tr2_sdmmc2_e4_block_scratch,
                sizeof(tr2_sdmmc2_e4_block_scratch));
            if (result == TR2_OK) {
                iface = campaign_data_store_bulk_interface(&active_store);
                result = iface->begin_campaign(
                    iface->context, TR2_SDMMC2_E4_CAMPAIGN_METADATA_CUT);
            }
            if (result == TR2_OK) {
                result = iface->append(
                    iface->context,
                    TR2_SDMMC2_E4_CAMPAIGN_METADATA_CUT,
                    tr2_sdmmc2_e4_record,
                    sizeof(tr2_sdmmc2_e4_record));
            }
            if (result == TR2_OK) {
                result = iface->checkpoint(
                    iface->context, TR2_SDMMC2_E4_CAMPAIGN_METADATA_CUT);
            }
            if (result == TR2_OK) {
                result = iface->append(
                    iface->context,
                    TR2_SDMMC2_E4_CAMPAIGN_METADATA_CUT,
                    tr2_sdmmc2_e4_record,
                    sizeof(tr2_sdmmc2_e4_record));
            }
            tr2_sdmmc2_e4_last_result = (uint32_t)result;
            if (result != TR2_OK) {
                tr2_sdmmc2_stage = 35U;
                return;
            }

            result = E4MarkerWrite(
                &media, TR2_SDMMC2_E4_MARKER_STATE_METADATA_CUT);
            if (result != TR2_OK) {
                tr2_sdmmc2_e4_last_result = (uint32_t)result;
                tr2_sdmmc2_stage = 36U;
                return;
            }
            injector.mode = TR2_E4_INJECT_AFTER_METADATA_WRITE;
            result = iface->checkpoint(
                iface->context, TR2_SDMMC2_E4_CAMPAIGN_METADATA_CUT);
            tr2_sdmmc2_e4_last_result = (uint32_t)result;
            tr2_sdmmc2_stage = 36U;
            return;
        }

        if (marker.state != TR2_SDMMC2_E4_MARKER_STATE_METADATA_CUT &&
            marker.state != TR2_SDMMC2_E4_MARKER_STATE_DONE) {
            tr2_sdmmc2_stage = 37U;
            return;
        }

        tr2_sdmmc2_e4_phase = 5U;
        tr2_sdmmc2_e4_recovery_status = (uint32_t)r402.status;
        tr2_sdmmc2_e4_recovered_prefix_bytes =
            r402.durable_prefix_bytes;
        if (r402.status != CAMPAIGN_DATA_RECOVERY_VALID ||
            (r402.durable_prefix_bytes != UINT64_C(16) &&
             r402.durable_prefix_bytes != UINT64_C(32))) {
            tr2_sdmmc2_stage = 37U;
            return;
        }
        /* DONE is idempotent; neither marker proves a physical cut occurred. */
        if (marker.state == TR2_SDMMC2_E4_MARKER_STATE_DONE) {
            tr2_sdmmc2_e4_phase = 6U;
            tr2_sdmmc2_stage = 38U;
            return;
        }
        result = E4MarkerWrite(&media, TR2_SDMMC2_E4_MARKER_STATE_DONE);
        tr2_sdmmc2_e4_last_result = (uint32_t)result;
        if (result != TR2_OK) {
            tr2_sdmmc2_stage = 37U;
            return;
        }
        tr2_sdmmc2_e4_phase = 6U;
        tr2_sdmmc2_stage = 38U;
        return;
    }

    /*
     * E3: compose the real D5-C CampaignDataStore with the real E1 adapter,
     * but only through the bounded sacrificial window.
     */
    {
        Tr2BulkMediaWindow window = {
            &media,
            (uint64_t)TR2_SDMMC2_E3_WINDOW_BLOCK * UINT64_C(512),
            TR2_SDMMC2_E3_WINDOW_SIZE
        };
        CampaignBulkMedia test_media = BulkWindowInterface(&window);
        CampaignDataStoreBulk store_a;
        CampaignDataStoreBulk store_b;
        CampaignDataStore *iface;
        CampaignDataRecoveryResult recovery;
        Tr2Result result;

        memset(tr2_sdmmc2_e3_metadata_clear,
               0,
               sizeof(tr2_sdmmc2_e3_metadata_clear));
        tr2_sdmmc2_e3_window_clear_result =
            (uint32_t)campaign_bulk_media_write(
                &test_media,
                0U,
                tr2_sdmmc2_e3_metadata_clear,
                sizeof(tr2_sdmmc2_e3_metadata_clear));
        if (tr2_sdmmc2_e3_window_clear_result != (uint32_t)TR2_OK) {
            tr2_sdmmc2_stage = 18U;
            return;
        }
        tr2_sdmmc2_e3_window_clear_sync_result =
            (uint32_t)campaign_bulk_media_sync(&test_media);
        if (tr2_sdmmc2_e3_window_clear_sync_result != (uint32_t)TR2_OK) {
            tr2_sdmmc2_stage = 19U;
            return;
        }

        for (size_t i = 0U; i < sizeof(tr2_sdmmc2_e3_checkpoint_data); ++i) {
            tr2_sdmmc2_e3_checkpoint_data[i] =
                (uint8_t)(((i * (size_t)13U) + (size_t)0x21U) & (size_t)0xFFU);
        }
        for (size_t i = 0U; i < sizeof(tr2_sdmmc2_e3_tail_data); ++i) {
            tr2_sdmmc2_e3_tail_data[i] =
                (uint8_t)(((i * (size_t)19U) + (size_t)0x71U) & (size_t)0xFFU);
        }

        result = campaign_data_store_bulk_init(
            &store_a,
            &test_media,
            tr2_sdmmc2_e3_payload_buffer_a,
            sizeof(tr2_sdmmc2_e3_payload_buffer_a),
            tr2_sdmmc2_e3_block_scratch_a,
            sizeof(tr2_sdmmc2_e3_block_scratch_a));
        tr2_sdmmc2_e3_store_init_result = (uint32_t)result;
        if (result != TR2_OK) {
            tr2_sdmmc2_stage = 20U;
            return;
        }

        iface = campaign_data_store_bulk_interface(&store_a);
        result = iface->begin_campaign(iface->context,
                                       TR2_SDMMC2_E3_CAMPAIGN_ID);
        tr2_sdmmc2_e3_begin_result = (uint32_t)result;
        if (result != TR2_OK) {
            tr2_sdmmc2_stage = 21U;
            return;
        }

        result = iface->append(iface->context,
                               TR2_SDMMC2_E3_CAMPAIGN_ID,
                               tr2_sdmmc2_e3_checkpoint_data,
                               sizeof(tr2_sdmmc2_e3_checkpoint_data));
        tr2_sdmmc2_e3_append_checkpoint_result = (uint32_t)result;
        if (result != TR2_OK) {
            tr2_sdmmc2_stage = 22U;
            return;
        }

        result = iface->checkpoint(iface->context,
                                   TR2_SDMMC2_E3_CAMPAIGN_ID);
        tr2_sdmmc2_e3_checkpoint_result = (uint32_t)result;
        if (result != TR2_OK) {
            tr2_sdmmc2_stage = 23U;
            return;
        }
        tr2_sdmmc2_e3_post_checkpoint_offset =
            campaign_bulk_block_writer_next_offset(&store_a.writer);

        result = iface->append(iface->context,
                               TR2_SDMMC2_E3_CAMPAIGN_ID,
                               tr2_sdmmc2_e3_tail_data,
                               sizeof(tr2_sdmmc2_e3_tail_data));
        tr2_sdmmc2_e3_append_tail_result = (uint32_t)result;
        if (result != TR2_OK) {
            tr2_sdmmc2_stage = 24U;
            return;
        }
        tr2_sdmmc2_e3_post_tail_offset =
            campaign_bulk_block_writer_next_offset(&store_a.writer);

        /*
         * Recreate the DataStore from the same physical medium without
         * publishing the tail. This models a reboot after the durable
         * checkpoint and proves that only its logical prefix is authority.
         */
        result = campaign_data_store_bulk_init(
            &store_b,
            &test_media,
            tr2_sdmmc2_e3_payload_buffer_b,
            sizeof(tr2_sdmmc2_e3_payload_buffer_b),
            tr2_sdmmc2_e3_block_scratch_b,
            sizeof(tr2_sdmmc2_e3_block_scratch_b));
        tr2_sdmmc2_e3_reboot_store_init_result = (uint32_t)result;
        if (result != TR2_OK) {
            tr2_sdmmc2_stage = 25U;
            return;
        }

        iface = campaign_data_store_bulk_interface(&store_b);
        result = iface->recover_campaign(iface->context,
                                         TR2_SDMMC2_E3_CAMPAIGN_ID,
                                         &recovery);
        tr2_sdmmc2_e3_recover_result = (uint32_t)result;
        tr2_sdmmc2_e3_recovery_status = (uint32_t)recovery.status;
        tr2_sdmmc2_e3_recovered_prefix_bytes =
            recovery.durable_prefix_bytes;
        if (result != TR2_OK ||
            recovery.status != CAMPAIGN_DATA_RECOVERY_VALID ||
            recovery.durable_prefix_bytes !=
                (uint64_t)TR2_SDMMC2_E3_CHECKPOINT_BYTES) {
            tr2_sdmmc2_stage = 26U;
            return;
        }

        if (tr2_sdmmc2_e3_post_checkpoint_offset !=
                TR2_CAMPAIGN_BULK_METADATA_BYTES +
                    UINT64_C(2) *
                        (uint64_t)TR2_CAMPAIGN_BULK_PHYSICAL_ALIGNMENT ||
            tr2_sdmmc2_e3_post_tail_offset !=
                tr2_sdmmc2_e3_post_checkpoint_offset +
                    (uint64_t)TR2_CAMPAIGN_BULK_PHYSICAL_ALIGNMENT) {
            tr2_sdmmc2_stage = 27U;
            return;
        }
    }

    tr2_sdmmc2_error_code = hsd2.ErrorCode;
    tr2_sdmmc2_stage = 28U;
}

static void Error_Handler(void);

void HAL_MspInit(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
}

/* Physical IIS3DWB diagnostic harness, isolated from persistent storage. */
volatile uint32_t tr2_iis3dwb_f1_init_result = UINT32_MAX;
volatile uint32_t tr2_iis3dwb_f1_start_result = UINT32_MAX;
volatile uint32_t tr2_iis3dwb_f1_sample_count = 0U;
volatile int32_t tr2_iis3dwb_f1_x_mg = 0;
volatile int32_t tr2_iis3dwb_f1_y_mg = 0;
volatile int32_t tr2_iis3dwb_f1_z_mg = 0;
volatile uint32_t tr2_iis3dwb_f1_saturated = 0U;

/* Hardware breakpoint here observes a complete XYZ snapshot. */
__attribute__((noinline)) void Iis3dwbF1SamplePublished(void)
{
    __NOP();
}

/* Diagnostic image only: never exposed as a production campaign publication. */
Iis3dwbDiagWindow tr2_iis3dwb_f2_diag;
volatile uint32_t tr2_iis3dwb_f2_result = UINT32_MAX;
volatile uint32_t tr2_iis3dwb_f2_ready = 0U;
volatile uint16_t tr2_iis3dwb_f2_b3_registers[TR2_B3_REGISTER_COUNT];

__attribute__((noinline)) void Iis3dwbF2DiagPublished(void)
{
    __NOP();
}

/* Explicit, immutable identity reserved by the operator for development bench 1.
   This is not the production FRAM identity/provisioning mechanism. */
static const IdentitySnapshot f3_bench_identity = {
    .generation = 1U,
    .device_id = UINT32_C(0x54520001),
    .hardware_version = 1U,
    .firmware_version_major = 0U,
    .firmware_version_minor = 3U,
    .firmware_version_patch = 0U,
    .protocol_version = 1U,
    .device_capabilities = UINT16_C(0x000D),
    .serial_number = "TR2-DEV-0001",
    .manufacturer = "MSM"
};

volatile uint32_t tr2_iis3dwb_f3_ready = 0U;
volatile uint32_t tr2_iis3dwb_f3_result = UINT32_MAX;
volatile uint32_t tr2_iis3dwb_f3_poll_errors = 0U;
/* GDB opt-in only. Never emitted automatically at boot or retried. */
volatile uint32_t tr2_iis3dwb_f3_probe_request = 0U;
volatile uint32_t tr2_iis3dwb_f3_probe_result = UINT32_MAX;
volatile uint32_t tr2_iis3dwb_f3_probe_count = 0U;

__attribute__((noinline)) void Iis3dwbF3ServerReady(void)
{
    __NOP();
}

static void Iis3dwbF3_Run(void)
{
    static SerialTransport transport;
    static Iis3dwbDiagModbus modbus;
    Tr2Result result = stm32_serial_transport_init_rs485(&transport);
    if (result == TR2_OK) {
        result = iis3dwb_diag_modbus_init(&modbus, &transport, 1U,
                                         &f3_bench_identity,
                                         &tr2_iis3dwb_f2_diag);
    }
    if (result == TR2_OK) {
        result = modbus_rtu_server_runtime_start(&modbus.server);
    }
    tr2_iis3dwb_f3_result = (uint32_t)result;
    if (result != TR2_OK) {
        for (;;) { HAL_Delay(100U); }
    }
    tr2_iis3dwb_f3_ready = 1U;
    Iis3dwbF3ServerReady();
    /* No acquisition during service: immutable diagnostic image, no P8 state.
       Drain IRQ events continuously; no delay that could overflow the RX queue. */
    for (;;) {
        if (tr2_iis3dwb_f3_probe_request != 0U) {
            uint32_t request = tr2_iis3dwb_f3_probe_request;
            tr2_iis3dwb_f3_probe_request = 0U;
            result = request == 1U ? iis3dwb_diag_modbus_transmit_probe(&modbus)
                                   : TR2_ERROR_INVALID_ARGUMENT;
            tr2_iis3dwb_f3_probe_result = (uint32_t)result;
            if (tr2_iis3dwb_f3_probe_count != UINT32_MAX) {
                ++tr2_iis3dwb_f3_probe_count;
            }
        }
        result = modbus_rtu_server_runtime_poll_once(&modbus.server);
        tr2_iis3dwb_f3_result = (uint32_t)result;
        if (result != TR2_OK && tr2_iis3dwb_f3_poll_errors != UINT32_MAX) {
            ++tr2_iis3dwb_f3_poll_errors;
        }
    }
}

static void Iis3dwbDiag_Run(void)
{
    static Stm32Iis3dwbVibrationSource source;
    VibrationSourceConfiguration configuration = {26667U, 0x0007U, 0U};
    VibrationSource interface;
    uint8_t who_am_i = 0U;
    Tr2Result result = stm32_iis3dwb_vibration_source_init(
        &source, &hspi3, TR2_IIS3DWB_CS_PORT, TR2_IIS3DWB_CS_PIN);

    tr2_iis3dwb_f1_init_result = (uint32_t)result;
    if (result == TR2_OK) {
        result = stm32_iis3dwb_vibration_source_read_who_am_i(&source, &who_am_i);
    }
    tr2_iis3dwb_whoami_status = (uint32_t)result;
    tr2_iis3dwb_whoami = who_am_i;
    tr2_iis3dwb_whoami_matches =
        (result == TR2_OK && who_am_i == TR2_IIS3DWB_WHO_AM_I_EXPECTED) ? 1U : 0U;
    interface = stm32_iis3dwb_vibration_source_interface(&source);
    if (tr2_iis3dwb_whoami_matches != 0U) {
        result = interface.configure(interface.context, &configuration);
        tr2_iis3dwb_config_status = (uint32_t)result;
        if (result == TR2_OK) {
            result = interface.start(interface.context);
            tr2_iis3dwb_f1_start_result = (uint32_t)result;
        }
    }
    /* Preserve initialization failure; no fallback or storage access. */
    while (result != TR2_OK || tr2_iis3dwb_whoami_matches == 0U) {
        HAL_Delay(100U);
    }
    HAL_Delay(10U);
    result = iis3dwb_diag_window_begin(&tr2_iis3dwb_f2_diag, HAL_GetTick());
    tr2_iis3dwb_f2_result = (uint32_t)result;
    if (result != TR2_OK) {
        for (;;) { HAL_Delay(100U); }
    }
    uint32_t last_sample_tick = HAL_GetTick();
    for (;;) {
        VibrationSample sample = {0};
        result = interface.read_sample(interface.context, &sample);
        tr2_iis3dwb_sample_status = (uint32_t)result;
        tr2_iis3dwb_data_ready = (result == TR2_OK && sample.valid) ? 1U : 0U;
        if (result == TR2_ERROR_NOT_AVAILABLE) {
            if ((uint32_t)(HAL_GetTick() - last_sample_tick) < 1000U) {
                continue;
            }
            result = TR2_ERROR_UNAVAILABLE;
        }
        tr2_iis3dwb_f2_result = (uint32_t)result;
        if (result != TR2_OK || !sample.valid) {
            if (result == TR2_OK) {
                tr2_iis3dwb_f2_result = (uint32_t)TR2_ERROR_NOT_AVAILABLE;
            }
            for (;;) {
                HAL_Delay(100U);
            }
        }
        tr2_iis3dwb_f1_x_mg = sample.x_mg;
        tr2_iis3dwb_f1_y_mg = sample.y_mg;
        tr2_iis3dwb_f1_z_mg = sample.z_mg;
        tr2_iis3dwb_f1_saturated = sample.saturated ? 1U : 0U;
        ++tr2_iis3dwb_f1_sample_count;
        last_sample_tick = HAL_GetTick();
        result = iis3dwb_diag_window_append(&tr2_iis3dwb_f2_diag, &sample);
        tr2_iis3dwb_f2_result = (uint32_t)result;
        if (result != TR2_OK) {
            for (;;) { HAL_Delay(100U); }
        }
        if (tr2_iis3dwb_f2_diag.window.valid_sample_count ==
            TR2_IIS3DWB_DIAG_WINDOW_SAMPLES) {
            result = interface.stop(interface.context);
            if (result == TR2_OK) {
                result = iis3dwb_diag_window_publish(&tr2_iis3dwb_f2_diag,
                                                    HAL_GetTick());
            }
            tr2_iis3dwb_f2_result = (uint32_t)result;
            if (result == TR2_OK) {
                for (size_t i = 0U; i < TR2_B3_REGISTER_COUNT; ++i) {
                    tr2_iis3dwb_f2_b3_registers[i] =
                        tr2_iis3dwb_f2_diag.image.registers[i];
                }
            }
            tr2_iis3dwb_f2_ready = result == TR2_OK ? 1U : 0U;
            Iis3dwbF2DiagPublished();
            if (result == TR2_OK) {
                Iis3dwbF3_Run();
            }
            /* Failed publication never becomes a favorable Modbus image. */
            for (;;) { HAL_Delay(100U); }
        }
    }
}

int main(void)
{
    SerialTransport serial_transport;

    HAL_Init();
    SystemClock_Config();
    SystemPower_Config();
    BringupLed_Init();
    Iis3dwbSpi_Init();
    Iis3dwbDiag_Run();

    FramSpi_Init();
    Sdmmc2_Bringup();

    {
        static Stm32Iis3dwbVibrationSource iis3dwb_source;
        VibrationSource vibration_source;
        VibrationSourceConfiguration configuration = {
            .sampling_frequency_hz = 26667U,
            .axes_enable_mask = 0x0007U,
            .full_scale_code = 0U
        };
        VibrationSample sample = {0};
        uint8_t who_am_i = 0U;
        Tr2Result result = stm32_iis3dwb_vibration_source_init(
            &iis3dwb_source, &hspi3, TR2_IIS3DWB_CS_PORT, TR2_IIS3DWB_CS_PIN);

        tr2_iis3dwb_spi_init_ok = (result == TR2_OK) ? 1U : 0U;
        if (result == TR2_OK) {
            result = stm32_iis3dwb_vibration_source_read_who_am_i(
                &iis3dwb_source, &who_am_i);
        }
        tr2_iis3dwb_whoami_status = (uint32_t)result;
        tr2_iis3dwb_whoami = who_am_i;
        tr2_iis3dwb_whoami_matches =
            (result == TR2_OK && who_am_i == TR2_IIS3DWB_WHO_AM_I_EXPECTED) ? 1U : 0U;

        vibration_source = stm32_iis3dwb_vibration_source_interface(&iis3dwb_source);
        if (tr2_iis3dwb_whoami_matches != 0U) {
            result = vibration_source.configure(vibration_source.context, &configuration);
            tr2_iis3dwb_config_status = (uint32_t)result;
            if (result == TR2_OK) {
                result = vibration_source.start(vibration_source.context);
            }
            if (result == TR2_OK) {
                HAL_Delay(10U);
                result = vibration_source.read_sample(vibration_source.context, &sample);
            }
            tr2_iis3dwb_sample_status = (uint32_t)result;
            if (result == TR2_OK) {
                tr2_iis3dwb_data_ready = sample.valid ? 1U : 0U;
                tr2_iis3dwb_raw_x = (int16_t)sample.x_mg;
                tr2_iis3dwb_raw_y = (int16_t)sample.y_mg;
                tr2_iis3dwb_raw_z = (int16_t)sample.z_mg;
                (void)vibration_source.stop(vibration_source.context);
            }
        }
    }

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
            (recovery.active_image == 1U)) {
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

#if TR2_FRAM_D3B_ALLOW_TIMING
        /*
         * One-shot D3-B timing campaign from the frozen D2-D final baseline.
         * The inactive image-A payload is safe scratch until publication:
         * recovery continues to use gen4/B if power is lost before commit.
         */
        if ((tr2_fram_d2_recover_result == (uint32_t)TR2_OK) &&
            (recovery.status == TRANSACTIONAL_IMAGE_RECOVERY_VALID) &&
            (recovery.generation == UINT64_C(6)) &&
            (recovery.active_image == 1U) &&
            (tr2_fram_d2_candidate[TR2_FRAM_D2C_TEST_OFFSET] ==
             UINT8_C(0xA8))) {
            PersistentMedia *persistent =
                transactional_image_media_interface(&media);
            uint32_t scratch_offset =
                geometry.image_a_base +
                TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE;
            uint32_t started;

            tr2_fram_d3b_attempted = 1U;

            started = HAL_GetTick();
            tr2_fram_d3b_read64_result = (uint32_t)physical.read(
                physical.context, scratch_offset,
                tr2_fram_d2_candidate, 64U);
            tr2_fram_d3b_read64_ms = HAL_GetTick() - started;

            started = HAL_GetTick();
            tr2_fram_d3b_write64_result = (uint32_t)physical.write(
                physical.context, scratch_offset,
                tr2_fram_d2_candidate, 64U);
            tr2_fram_d3b_write64_ms = HAL_GetTick() - started;

            started = HAL_GetTick();
            tr2_fram_d3b_read_full_result = (uint32_t)physical.read(
                physical.context, scratch_offset,
                tr2_fram_d2_candidate,
                TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
            tr2_fram_d3b_read_full_ms = HAL_GetTick() - started;

            started = HAL_GetTick();
            tr2_fram_d3b_write_full_result = (uint32_t)physical.write(
                physical.context, scratch_offset,
                tr2_fram_d2_candidate,
                TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
            tr2_fram_d3b_write_full_ms = HAL_GetTick() - started;

            if ((persistent != NULL) &&
                (tr2_fram_d3b_read64_result == (uint32_t)TR2_OK) &&
                (tr2_fram_d3b_write64_result == (uint32_t)TR2_OK) &&
                (tr2_fram_d3b_read_full_result == (uint32_t)TR2_OK) &&
                (tr2_fram_d3b_write_full_result == (uint32_t)TR2_OK)) {
                uint8_t value = TR2_FRAM_D3B_TEST_VALUE;
                TransactionalImageRecoveryResult post = {
                    .status = TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE,
                    .generation = UINT64_C(0),
                    .active_image = 0xFFU
                };

                if (persistent->write(
                        persistent->context,
                        TR2_FRAM_D2C_TEST_OFFSET,
                        &value,
                        sizeof(value)) == TR2_OK) {
                    started = HAL_GetTick();
                    tr2_fram_d3b_commit_result =
                        (uint32_t)persistent->commit(persistent->context);
                    tr2_fram_d3b_commit_ms = HAL_GetTick() - started;

                    if (tr2_fram_d3b_commit_result == (uint32_t)TR2_OK) {
                        tr2_fram_d3b_post_recover_result =
                            (uint32_t)transactional_image_media_recover(
                                &media, &post);
                        if (tr2_fram_d3b_post_recover_result ==
                            (uint32_t)TR2_OK) {
                            tr2_fram_d3b_post_status = (uint32_t)post.status;
                            tr2_fram_d3b_post_generation = post.generation;
                            tr2_fram_d3b_post_active_image = post.active_image;
                            if (post.status ==
                                TRANSACTIONAL_IMAGE_RECOVERY_VALID) {
                                tr2_fram_d3b_post_value =
                                    tr2_fram_d2_candidate[
                                        TR2_FRAM_D2C_TEST_OFFSET];
                                tr2_fram_d3b_completed = 1U;
                                HAL_GPIO_WritePin(
                                    TR2_BRINGUP_LED_PORT,
                                    TR2_BRINGUP_LED_PIN,
                                    GPIO_PIN_SET);
                                __disable_irq();
                                for (;;) {
                                }
                            }
                        }
                    }
                }
            }
        }
#endif

#if TR2_FRAM_D3C_ALLOW_ALTERNATION
        /*
         * D3-C executes exactly three normal commits from the explicitly
         * observed D3-B baseline gen7/A/0xA9. Each commit is followed by a
         * normal recovery and records generation, image, publication copy and
         * readback value. No format, repair or retry is performed.
         */
        if ((tr2_fram_d2_recover_result == (uint32_t)TR2_OK) &&
            (recovery.status == TRANSACTIONAL_IMAGE_RECOVERY_VALID) &&
            (recovery.generation == UINT64_C(7)) &&
            (recovery.active_image == 0U) &&
            (tr2_fram_d2_candidate[TR2_FRAM_D2C_TEST_OFFSET] == UINT8_C(0xA9))) {
            static const uint8_t values[TR2_FRAM_D3C_COMMIT_COUNT] = {
                UINT8_C(0xAA), UINT8_C(0xAB), UINT8_C(0xAC)};
            PersistentMedia *persistent = transactional_image_media_interface(&media);

            if (persistent != NULL) {
                tr2_fram_d3c_attempted = 1U;
                for (uint32_t i = 0U; i < TR2_FRAM_D3C_COMMIT_COUNT; ++i) {
                    TransactionalImageRecoveryResult post = {
                        .status = TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE,
                        .generation = UINT64_C(0),
                        .active_image = 0xFFU
                    };
                    Tr2Result write_result = persistent->write(
                        persistent->context, TR2_FRAM_D2C_TEST_OFFSET,
                        &values[i], sizeof(values[i]));
                    if (write_result != TR2_OK) {
                        tr2_fram_d3c_commit_result[i] = (uint32_t)write_result;
                        break;
                    }

                    tr2_fram_d3c_commit_result[i] =
                        (uint32_t)persistent->commit(persistent->context);
                    if (tr2_fram_d3c_commit_result[i] != (uint32_t)TR2_OK) {
                        break;
                    }

                    tr2_fram_d3c_recover_result[i] =
                        (uint32_t)transactional_image_media_recover(&media, &post);
                    if ((tr2_fram_d3c_recover_result[i] != (uint32_t)TR2_OK) ||
                        (post.status != TRANSACTIONAL_IMAGE_RECOVERY_VALID)) {
                        break;
                    }

                    tr2_fram_d3c_generation[i] = post.generation;
                    tr2_fram_d3c_active_image[i] = post.active_image;
                    tr2_fram_d3c_active_superblock[i] = media.active_superblock;
                    tr2_fram_d3c_value[i] =
                        tr2_fram_d2_candidate[TR2_FRAM_D2C_TEST_OFFSET];
                }

                if ((tr2_fram_d3c_generation[0] == UINT64_C(8)) &&
                    (tr2_fram_d3c_generation[1] == UINT64_C(9)) &&
                    (tr2_fram_d3c_generation[2] == UINT64_C(10)) &&
                    (tr2_fram_d3c_active_image[0] == 1U) &&
                    (tr2_fram_d3c_active_image[1] == 0U) &&
                    (tr2_fram_d3c_active_image[2] == 1U) &&
                    (tr2_fram_d3c_active_superblock[0] == 1U) &&
                    (tr2_fram_d3c_active_superblock[1] == 0U) &&
                    (tr2_fram_d3c_active_superblock[2] == 1U) &&
                    (tr2_fram_d3c_value[0] == UINT8_C(0xAA)) &&
                    (tr2_fram_d3c_value[1] == UINT8_C(0xAB)) &&
                    (tr2_fram_d3c_value[2] == UINT8_C(0xAC))) {
                    tr2_fram_d3c_completed = 1U;
                    HAL_GPIO_WritePin(TR2_BRINGUP_LED_PORT,
                                      TR2_BRINGUP_LED_PIN, GPIO_PIN_SET);
                    __disable_irq();
                    for (;;) {
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

static void Iis3dwbSpi_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_SPI3_CLK_ENABLE();

    HAL_GPIO_WritePin(TR2_IIS3DWB_CS_PORT, TR2_IIS3DWB_CS_PIN, GPIO_PIN_SET);

    gpio.Pin = TR2_IIS3DWB_CS_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TR2_IIS3DWB_CS_PORT, &gpio);

    gpio.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(GPIOC, &gpio);

    hspi3.Instance = SPI3;
    hspi3.Init.Mode = SPI_MODE_MASTER;
    hspi3.Init.Direction = SPI_DIRECTION_2LINES;
    hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi3.Init.NSS = SPI_NSS_SOFT;
    hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi3.Init.CRCPolynomial = 0x7U;
    hspi3.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    hspi3.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hspi3.Init.FifoThreshold = SPI_FIFO_THRESHOLD_08DATA;
    hspi3.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
    hspi3.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    hspi3.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    hspi3.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;
    hspi3.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    hspi3.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
    hspi3.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;

    if (HAL_SPI_Init(&hspi3) != HAL_OK) {
        Error_Handler();
    }

    tr2_iis3dwb_spi_init_ok = 1U;
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
