/* M014-H dedicated production composition. No qualification harness or SD access.
 * Storage is explicitly unavailable; persisted configuration is structurally
 * checked off-boot, but normal boot cannot activate it without resource authority. */
#include "stm32_production_board.h"
#include "stm32_serial_transport.h"
#include "stm32_modbus_application.h"
#include "tr2_generated_commissioning.h"
#include <string.h>
#if TR2_STM32_FIFO_COOPERATIVE
#include "stm32_fifo_cooperative.h"
#include "stm32_irq_observer.h"
#endif

static SPI_HandleTypeDef *fram_spi, *sensor_spi;
static Stm32Iis3dwbVibrationSource sensor_probe;
static CampaignDataStore unavailable_store;
static bool initialized;
volatile uint32_t tr2_production_stage, tr2_production_result=UINT32_MAX;
volatile uint32_t tr2_production_poll_errors, tr2_production_poll_count;
volatile uint32_t tr2_production_silence_events;
volatile uint32_t tr2_provision_trigger, tr2_provision_stage;
volatile uint32_t tr2_provision_result=UINT32_MAX;
volatile uint64_t tr2_provision_generation;
uint8_t tr2_provision_before[262144];
uint8_t tr2_provision_candidate[51914];
uint8_t tr2_provision_desired[51914];
static Stm32FramStorage manufacturing_fram;
static TransactionalImageMedia manufacturing_media;
static ProductionCommissioningAuthorities structural_authority;
static uint32_t crc32(const uint8_t *bytes,size_t size)
{
    uint32_t crc=UINT32_MAX; size_t i; unsigned bit;
    for(i=0;i<size;++i){crc^=bytes[i];for(bit=0;bit<8;++bit)crc=(crc&1u)?(crc>>1)^UINT32_C(0xEDB88320):crc>>1;}
    return crc^UINT32_MAX;
}
static Tr2Result unavailable_action(void *context,CampaignId id)
{(void)context;(void)id;return TR2_ERROR_NOT_AVAILABLE;}
static Tr2Result unavailable_append(void *context,CampaignId id,const uint8_t *data,size_t size)
{(void)data;(void)size;return unavailable_action(context,id);}
static Tr2Result unavailable_recover(void *context,CampaignId id,CampaignDataRecoveryResult *out)
{
    (void)context;(void)id;if(out==NULL)return TR2_ERROR_INVALID_ARGUMENT;
    *out=(CampaignDataRecoveryResult){CAMPAIGN_DATA_RECOVERY_UNAVAILABLE,0};return TR2_OK;
}
static bool storage_available(void *context){(void)context;return false;}
static Tr2Result preboot_authority(void *context,const uint8_t *image,size_t size,
    const PersistentIdentityRecord *identity,const ModbusBlock0Image *b0)
{
    Tr2Result result;(void)context;
    if(size!=51914||identity->identity.unit_id!=tr2_board_identity.identity.unit_id||
       memcmp(b0->registers,tr2_board_expected_b0,sizeof(tr2_board_expected_b0))!=0)return TR2_ERROR_INVALID_STATE;
    result=production_commissioning_check_attribution(&structural_authority,
        identity->identity.hardware_uid,identity->identity.functional.device_id);
    if(result!=TR2_OK)return result;
    return production_commissioning_inspect_prefix(&structural_authority,image,51818);
}
static TransactionalImageGeometry geometry(void)
{
    /* Explicit reviewed V2 geometry, not a harness allocation. */
    return (TransactionalImageGeometry){262144,0,4096,8192,65536,57344};
}
void stm32_production_board_initialize(SPI_HandleTypeDef *fram,SPI_HandleTypeDef *sensor)
{
    fram_spi=fram;sensor_spi=sensor;
    unavailable_store=(CampaignDataStore){NULL,unavailable_action,unavailable_append,
        unavailable_action,unavailable_action,unavailable_recover};
    /* A permissive resource ceiling is ONLY for structural domain validation.
     * It is never passed to normal boot or claimed as usable storage capacity. */
    structural_authority.environment=(ConfigurationValidationEnvironment){true,UINT32_MAX};
    structural_authority.assignments=&tr2_board_assignment;
    structural_authority.assignment_count=1;
    initialized=fram!=NULL&&sensor!=NULL;
}
Tr2Result stm32_production_binding_acquire(SerialTransport *uart,
    Stm32ProductionDependencies *deps,SerialTransport **qualified_transport)
{
    uint32_t uid[3]={HAL_GetUIDw0(),HAL_GetUIDw1(),HAL_GetUIDw2()};uint8_t who=0;
    Tr2Result result;
    if(!initialized||deps==NULL||qualified_transport==NULL||!serial_transport_is_valid(uart))return TR2_ERROR_INVALID_STATE;
    result=production_commissioning_check_attribution(&structural_authority,uid,tr2_board_assignment.device_id);
    if(result!=TR2_OK)return result;
    result=stm32_iis3dwb_vibration_source_init(&sensor_probe,sensor_spi,GPIOC,GPIO_PIN_9);
    if(result==TR2_OK)result=stm32_iis3dwb_vibration_source_read_who_am_i(&sensor_probe,&who);
    if(result!=TR2_OK||who!=0x7B)return TR2_ERROR_NOT_AVAILABLE;
    *deps=(Stm32ProductionDependencies){0};deps->fram_spi=fram_spi;
    deps->fram_cs_port=GPIOD;deps->fram_cs_pin=GPIO_PIN_14;
    deps->vibration_spi=sensor_spi;deps->vibration_cs_port=GPIOC;deps->vibration_cs_pin=GPIO_PIN_9;
    deps->geometry=geometry();deps->exclusive_media_ownership=true;
    deps->protocol_authority_available=true;deps->supported_protocol_version=1;
    deps->campaign_data_store=&unavailable_store;deps->storage_is_available=storage_available;
    deps->environment=(ConfigurationValidationEnvironment){false,0};
    deps->preboot_authority=preboot_authority;
    *qualified_transport=uart;return TR2_OK;
}
static Tr2Result manufacturing_preflight(void)
{
    TransactionalImagePhysicalStorage physical;TransactionalImageRecoveryResult recovery;
    TransactionalImageGeometry g=geometry();
    uint32_t uid[3]={HAL_GetUIDw0(),HAL_GetUIDw1(),HAL_GetUIDw2()};uint8_t verify[512];size_t i;
    Tr2Result result;
    if(!initialized)return TR2_ERROR_INVALID_STATE;
    result=production_commissioning_check_attribution(&structural_authority,uid,tr2_board_assignment.device_id);
    if(result!=TR2_OK)return result;
    result=stm32_fram_storage_init(&manufacturing_fram,fram_spi,GPIOD,GPIO_PIN_14,100);
    if(result==TR2_OK)result=stm32_fram_storage_read(&manufacturing_fram,0,tr2_provision_before,sizeof(tr2_provision_before));
    if(result!=TR2_OK)return result;
    for(i=0;i<sizeof(tr2_provision_before);i+=sizeof(verify)){
        result=stm32_fram_storage_read(&manufacturing_fram,(uint32_t)i,verify,sizeof(verify));
        if(result!=TR2_OK||memcmp(verify,tr2_provision_before+i,sizeof(verify))!=0)return TR2_ERROR_STORAGE;
    }
    if(crc32(tr2_provision_before,sizeof(tr2_provision_before))!=TR2_EXPECTED_FRAM_CRC)return TR2_ERROR_INVALID_STATE;
    physical=stm32_fram_storage_physical(&manufacturing_fram);
    result=transactional_image_media_init_v2(&manufacturing_media,&physical,&g,tr2_provision_candidate,sizeof(tr2_provision_candidate));
    if(result==TR2_OK)result=production_provisioning_probe(&manufacturing_media,&recovery);
    if(result!=TR2_OK)return result;
    tr2_provision_generation=recovery.generation;
    if(recovery.status!=TRANSACTIONAL_IMAGE_RECOVERY_VALID||recovery.generation!=10||manufacturing_media.logical_version!=1)return TR2_ERROR_INVALID_STATE;
    /* Candidate-only structural validation before authorizing the first write. */
    memcpy(tr2_provision_desired,tr2_provision_candidate,51818);
    result=tr2_configuration_record_encode(&tr2_board_configuration,tr2_provision_desired+140,140);
    if(result!=TR2_OK)return result;
    for(i=140;i<280;++i)if(tr2_provision_candidate[i]!=0)return TR2_ERROR_INVALID_STATE;
    result=production_commissioning_inspect_prefix(&structural_authority,tr2_provision_desired,51818);
    return result;
}
static Tr2Result manufacturing_apply(void)
{
    ProductionProvisioningResult out;ProductionCommissioningConfigurationRequest q={0};
    PersistentIdentityProvisioningRequest identity={0};Tr2Result result;
    q.authorities=&structural_authority;q.configuration=&tr2_board_configuration;
    q.observed_uid[0]=HAL_GetUIDw0();q.observed_uid[1]=HAL_GetUIDw1();q.observed_uid[2]=HAL_GetUIDw2();
    memcpy(q.target_uid,tr2_board_assignment.hardware_uid,sizeof(q.target_uid));
    q.device_id=tr2_board_assignment.device_id;q.expected_generation=10;q.exclusive_ownership=true;
    /* Re-read full preflight immediately after the single explicit RAM trigger. */
    result=manufacturing_preflight();if(result!=TR2_OK)return result;
    tr2_provision_stage=3;
    result=production_commissioning_install_configuration(&manufacturing_media,&q,true,&out);
    if(result!=TR2_OK)return result;
    tr2_provision_generation=out.recovery.generation;
    if(out.unchanged||out.recovery.generation!=11)return TR2_ERROR_INVALID_STATE;
    memcpy(tr2_provision_desired,tr2_provision_candidate,51818);
    result=persistent_identity_encode(&tr2_board_identity,tr2_provision_desired+51818);
    if(result!=TR2_OK)return result;
    identity.context=&structural_authority;identity.attribution=production_commissioning_check_attribution;
    identity.validate_prefix=production_commissioning_inspect_prefix;
    memcpy(identity.target_uid,tr2_board_assignment.hardware_uid,sizeof(identity.target_uid));
    identity.supported_protocol_version=1;identity.expected_generation=11;identity.exclusive_ownership=true;
    identity.image=tr2_provision_desired;identity.image_size=sizeof(tr2_provision_desired);
    tr2_provision_stage=4;
    result=persistent_identity_provision(&manufacturing_media,&identity,true,&out);
    tr2_provision_generation=out.recovery.generation;
    if(result==TR2_OK&&out.recovery.generation!=12)return TR2_ERROR_INVALID_STATE;
    return result;
}
__attribute__((noreturn)) void stm32_production_manufacturing_run(void)
{
    tr2_provision_stage=1;tr2_provision_result=(uint32_t)manufacturing_preflight();
    if(tr2_provision_result==TR2_OK){
        tr2_provision_stage=2;
        while(tr2_provision_trigger==0)HAL_Delay(1);
        if(tr2_provision_trigger==UINT32_C(0x4D313448))tr2_provision_result=(uint32_t)manufacturing_apply();
        else tr2_provision_result=TR2_ERROR_INVALID_ARGUMENT;
    }
    tr2_provision_stage=5;
    for(;;)HAL_Delay(250);
}
__attribute__((noreturn)) void stm32_production_server_run(void)
{
    static SerialTransport transport;static ModbusSystemServer server;
    ModbusSystemServerBinding binding={0};Tr2Result result;uint32_t tick;
    tr2_production_stage=1;
    result=stm32_serial_transport_init(&transport);
    if(result==TR2_OK)result=stm32_modbus_application_bind(&transport,&binding);
    /* Persistent identity is authoritative, reviewed expected B0 is only a guard. */
    if(result==TR2_OK&&(binding.identity==NULL||binding.unit_id!=tr2_board_identity.identity.unit_id||
        memcmp(binding.identity->registers,tr2_board_expected_b0,sizeof(tr2_board_expected_b0))!=0))result=TR2_ERROR_INVALID_STATE;
    binding.refresh_status=system_runtime_build_boot_status_images;
    if(result==TR2_OK)result=modbus_system_server_init(&server,&binding);
    if(result==TR2_OK)result=modbus_system_server_start(&server);
    tr2_production_result=(uint32_t)result;
    if(result!=TR2_OK){tr2_production_stage=99;for(;;)HAL_Delay(250);}
    tr2_production_stage=2;
#if TR2_STM32_FIFO_COOPERATIVE
    stm32_fifo_cooperative_prepare(sensor_spi,fram_spi);
    server.binding.command_io_available=stm32_fifo_cooperative_command_io_available;
#endif
    for(;;){
        tick=HAL_GetTick();
#if TR2_STM32_PRODUCTION_SILENCE_TEST
        /* One qualification window only; drains incoming events so loss request
         * cannot become a delayed response after the window. No reset/GDB needed. */
        if(tick>=90000&&tick<180000){
            SerialTransportEvent event;
            modbus_rtu_receiver_init(&server.rtu.receiver);
            if(!serial_transport_poll_event(&transport,&event))++tr2_production_poll_errors;
            else if(event.type!=SERIAL_TRANSPORT_EVENT_NONE)++tr2_production_silence_events;
            tr2_production_stage=3;continue;
        }
#endif
        (void)tick;tr2_production_stage=2;
#if TR2_STM32_FIFO_COOPERATIVE
        const uint32_t before_modbus_cycles=DWT->CYCCNT;
        const uint32_t before_t3_5=tr2_irq_observer.t3_5_polled;
        const size_t frame_length=server.rtu.receiver.length;
        const ModbusRtuReceiverState receiver_state=server.rtu.receiver.state;
#endif
        result=modbus_system_server_poll_once(&server);++tr2_production_poll_count;
        if(result!=TR2_OK)++tr2_production_poll_errors;
#if TR2_STM32_FIFO_COOPERATIVE
        stm32_fifo_cooperative_observe_service(DWT->CYCCNT-before_modbus_cycles,
            server.command_io_denied_requests,server.command_snapshot_failures);
        /* Passive observer uses the existing decoded-frame boundary, without
         * replacing production prepare/binding callbacks or their authority. */
        if(before_t3_5!=tr2_irq_observer.t3_5_polled &&
           (receiver_state==MODBUS_RTU_RECEIVER_RECEIVING || receiver_state==MODBUS_RTU_RECEIVER_WAITING_T3_5)){
            ModbusRtuAduView request;
            if(modbus_rtu_adu_decode(server.rtu.receiver.buffer,frame_length,&request)==MODBUS_RTU_CODEC_OK && request.unit_id==server.rtu.local_unit_id)
                stm32_fifo_cooperative_observe(&request);
        }
        stm32_fifo_cooperative_poll();
#endif
    }
}
