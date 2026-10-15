/* R4 opt-in qualification only. No SD, UART, production entry or boot write.
 * One explicit bench allocation, backed up twice before flash. It cannot grant
 * protocol2 or B3 authority: no sample-time provider is bound. */
#include "stm32_live_acquisition.h"
#include "stm32_fram_storage.h"
#include "tr2/common/cooperative_work.h"
#include "tr2/common/sha256.h"
#include "tr2/modbus/b4_configuration_codec.h"
#include <string.h>

typedef struct {
 uint32_t stage,error,line,command,ownership,read_passes,commands;
 uint32_t core_hz,spi1_hz,spi3_hz,tick_start,tick_end;
 uint64_t max_read_us,max_write_us,max_gap_us,last_poll_us,safety_us,first_data_us,start_us;
 uint64_t read_bytes,write_bytes,xyz,timestamps,polls,elapsed_us;
 uint32_t max_level,fifo_error,stop_error,engine_status,engine_result;
 uint32_t protected_equal,baseline_equal;
} R4BenchResult;
volatile R4BenchResult r4_result;
volatile uint32_t r4_trigger;
static uint8_t baseline[262144],candidate[52298];
static Stm32FramStorage fram;
static TransactionalImagePhysicalStorage bounded;
static TransactionalImageMedia historical,live;
static LiveConfiguration configuration;
static LiveCommandStore store;
static LiveCommandEngine engine;
static Stm32LiveAcquisition owner;
static Tr2CooperativeWork work;
static uint32_t previous_cycles;
static uint64_t elapsed_cycles;
static const uint32_t uid[3]={0x004A003D,0x38425018,0x20333157};
static const uint8_t baseline_sha[32]={
 0xd7,0x32,0xac,0xc5,0x57,0x15,0x21,0x80,0xfc,0x39,0x40,0x97,0xd6,0xa5,0x26,0x9e,
 0xa2,0xf4,0x5c,0x1d,0xa3,0x87,0xc8,0x64,0x3f,0x51,0x23,0x18,0xad,0xf9,0x2f,0xce};
static uint64_t now_us(void *unused)
{
 (void)unused;uint32_t cycles=DWT->CYCCNT;
 elapsed_cycles+=(uint32_t)(cycles-previous_cycles);previous_cycles=cycles;
 return elapsed_cycles/160u;
}
static int consume(void *ctx,const uint8_t *words,uint32_t n,uint64_t index)
{(void)ctx;(void)words;(void)n;(void)index;if(!r4_result.first_data_us)r4_result.first_data_us=now_us(NULL)-r4_result.start_us;return 0;}
static void stop(void *unused)
{
 (void)unused;uint64_t start=now_us(NULL);
 (void)fifo_live_stop(&owner.acquisition.fifo);
 uint64_t duration=now_us(NULL)-start;
 if(duration>r4_result.safety_us)r4_result.safety_us=duration;
}
static void snapshot(void)
{
 r4_result.xyz=owner.acquisition.fifo.total_xyz;
 r4_result.timestamps=owner.acquisition.fifo.total_ts;
 r4_result.max_level=owner.acquisition.fifo.parser.max_level;
 r4_result.fifo_error=owner.acquisition.fifo.error;
 r4_result.stop_error=owner.acquisition.fifo.stop_error;
 r4_result.elapsed_us=now_us(NULL);r4_result.tick_end=HAL_GetTick();
 r4_result.engine_status=engine.status;r4_result.engine_result=engine.result;
}
static Tr2Result service(void *unused)
{
 (void)unused;
 if(!owner.acquisition.fifo.running)return TR2_OK;
 uint64_t t=now_us(NULL);
 if(r4_result.last_poll_us&&t-r4_result.last_poll_us>r4_result.max_gap_us)
  r4_result.max_gap_us=t-r4_result.last_poll_us;
 r4_result.last_poll_us=t;r4_result.polls++;
 return stm32_live_acquisition_poll(&owner);
}
static Tr2Result read_physical(void *ctx,uint32_t at,void *buffer,size_t n)
{
 (void)ctx;uint64_t t=now_us(NULL);Tr2Result r=bounded.read(bounded.context,at,buffer,n);
 t=now_us(NULL)-t;if(t>r4_result.max_read_us)r4_result.max_read_us=t;
 r4_result.read_bytes+=n;return r;
}
static bool identity_ok(void)
{return HAL_GetUIDw0()==uid[0]&&HAL_GetUIDw1()==uid[1]&&HAL_GetUIDw2()==uid[2];}
static Tr2Result ownership(void *ctx,uint32_t at,size_t n)
{
 (void)ctx;
 /* Exclusive single main-stack owner; no transport/SD task exists in this image.
  * Flag only follows exact full SHA check against both verified fresh backups. */
 return r4_result.ownership&&identity_ok()&&at>=122880u&&at<=245760u&&n<=245760u-at
  ?TR2_OK:TR2_ERROR_UNAVAILABLE;
}
static Tr2Result write_physical(void *ctx,uint32_t at,const void *buffer,size_t n)
{
 (void)ctx;if(ownership(NULL,at,n)!=TR2_OK)return TR2_ERROR_UNAVAILABLE;
 uint64_t t=now_us(NULL);Tr2Result r=bounded.write(bounded.context,at,buffer,n);
 t=now_us(NULL)-t;if(t>r4_result.max_write_us)r4_result.max_write_us=t;
 r4_result.write_bytes+=n;return r;
}
static bool admission(void *ctx,uint16_t code)
{
 (void)ctx;(void)code;
 /* Bench-only experimental admission. 1ms+2.5ms gives 106 incoming words;
  * last hardware status plus that reserve must remain strictly below512. */
 return tr2_cooperative_status(&work)==TR2_OK&&r4_result.ownership&&
  owner.acquisition.fifo.parser.last_level+106u<512u;
}
#define REQUIRE(expression) do {Tr2Result status=(expression);if(status!=TR2_OK){r4_result.error=status;r4_result.line=__LINE__;return status;}}while(0)
static Tr2Result read_baseline(void)
{
 uint8_t digest[32];
 REQUIRE(tr2_cooperative_enter(&work));
 for(uint32_t at=0;at<sizeof(baseline);at+=32) {
  REQUIRE(read_physical(NULL,at,baseline+at,32));
  REQUIRE(tr2_cooperative_checkpoint(&work));
 }
 REQUIRE(tr2_cooperative_leave(&work));
 REQUIRE(tr2_sha256_bytes_cooperative(baseline,sizeof(baseline),digest,&work));
 if(memcmp(digest,baseline_sha,32))return TR2_ERROR_STORAGE;
 r4_result.baseline_equal=1;r4_result.read_passes++;return TR2_OK;
}
static Tr2Result compare_protected(void)
{
 uint8_t bytes[32];REQUIRE(tr2_cooperative_enter(&work));
 for(uint32_t at=0;at<262144u;at+=32) {
  if(at>=122880u&&at<245760u)continue;
  REQUIRE(read_physical(NULL,at,bytes,32));
  if(memcmp(bytes,baseline+at,32))return TR2_ERROR_STORAGE;
  REQUIRE(tr2_cooperative_checkpoint(&work));
 }
 REQUIRE(tr2_cooperative_leave(&work));r4_result.protected_equal=1;return TR2_OK;
}
static PreparedConfiguration prepared(void)
{
 PreparedConfiguration q={0};q.config_id=2;
 q.payload.sampling_frequency_hz=26667;q.payload.axes_enable_mask=7;
 q.payload.full_scale_code=0;q.payload.acquisition_mode=2;
 q.payload.window_size_samples=4096;q.payload.indicator_period_ms=2000;
 q.payload.supervision_enable_mask=1;q.payload.rms_alarm_threshold_mg=1000;
 q.payload.peak_alarm_threshold_mg=2000;q.payload.campaign_context_id=1;
 q.payload.mission_id=4;memcpy(q.payload.campaign_label,"R4-BENCH",8);
 memcpy(q.payload.mission_label,"FRAM-FIFO",9);
 q.supplied_crc=tr2_b4_prepared_payload_crc(&q.payload);return q;
}
static Tr2Result command(uint32_t epoch,uint16_t txid,uint16_t code)
{
 LiveCommandRequest q={0};q.epoch=epoch;q.command.transaction_id=txid;
 q.command.identity.command_code=code;
 REQUIRE(live_command_engine_submit(&engine,&q));
 for(unsigned i=0;engine.has_active&&i<5;i++)REQUIRE(live_command_engine_step(&engine));
 snapshot();
 if(engine.has_active||engine.status!=4||engine.result)return TR2_ERROR_INVALID_STATE;
 r4_result.commands++;return TR2_OK;
}
static Tr2Result qualify(uint32_t mode)
{
 r4_result.stage=2;REQUIRE(read_baseline());
 r4_result.stage=3;
 if(fifo_live_stop(&owner.acquisition.fifo))return TR2_ERROR_UNAVAILABLE;
 if(mode==1) {
  r4_result.start_us=now_us(NULL);
  if(fifo_live_start(&owner.acquisition.fifo,1,1,1,7,0,HAL_GetTick()))return TR2_ERROR_UNAVAILABLE;
  REQUIRE(tr2_cooperative_enter(&work));
  uint64_t until=now_us(NULL)+5000000u;
  do {
   for(uint32_t at=0;at<122880u;at+=32) {
    uint8_t bytes[32];REQUIRE(read_physical(NULL,at,bytes,32));
    if(memcmp(bytes,baseline+at,32))return TR2_ERROR_STORAGE;
    REQUIRE(tr2_cooperative_checkpoint(&work));
   }
  } while(now_us(NULL)<until);
  REQUIRE(tr2_cooperative_leave(&work));stop(NULL);snapshot();
  REQUIRE(compare_protected());return TR2_OK;
 }
 /* Ownership grant is local to this explicitly triggered, verified allocation. */
 r4_result.ownership=1;
 TransactionalImagePhysicalStorage storage={NULL,read_physical,write_physical};
 TransactionalImageGeometry geometry=transactional_image_geometry_qualification_profile();
 PersistentIdentityAssignment assignment={ {0x004A003D,0x38425018,0x20333157},1};
 REQUIRE(transactional_image_media_init_v2(&historical,&storage,&geometry,candidate,sizeof(candidate)));
 REQUIRE(transactional_image_media_init_live(&live,&storage,candidate,sizeof(candidate),NULL,ownership));
 REQUIRE(transactional_image_media_bind_work(&historical,&work));
 REQUIRE(transactional_image_media_bind_work(&live,&work));
 REQUIRE(live_configuration_init(&configuration,&historical,&live,uid,&assignment,1));
 r4_result.stage=4;REQUIRE(live_configuration_migrate(&configuration));
 PreparedConfiguration q=prepared();REQUIRE(live_configuration_apply(&configuration,&q,false));
 REQUIRE(live_command_store_init(&store,&configuration));
 owner.acquisition.budget=admission;owner.acquisition.budget_context=NULL;
 LiveAcquisitionPort port=live_fifo_acquisition_port(&owner.acquisition);
 REQUIRE(live_command_engine_init(&engine,&store,&port));
 owner.acquisition.engine=&engine; /* Raw qualification only: B3 remains unbound. */
 REQUIRE(live_command_engine_boot(&engine));
 r4_result.stage=5;r4_result.start_us=now_us(NULL);REQUIRE(command(1,1,13));
 r4_result.stage=6;REQUIRE(command(1,65535,12));
 r4_result.stage=7;
 REQUIRE(tr2_cooperative_enter(&work));uint64_t until=now_us(NULL)+60000000u;
 while(now_us(NULL)<until)REQUIRE(tr2_cooperative_checkpoint(&work));
 REQUIRE(tr2_cooperative_leave(&work));
 r4_result.stage=8;REQUIRE(command(2,2,14));
 r4_result.stage=9;q.config_id=3;q.supplied_crc=tr2_b4_prepared_payload_crc(&q.payload);
 REQUIRE(live_command_engine_prepare(&engine,&q));REQUIRE(command(2,3,1));
 snapshot();REQUIRE(compare_protected());r4_result.ownership=0;return TR2_OK;
}
__attribute__((noreturn)) void stm32_r4_benchmark_run(SPI_HandleTypeDef *spi1,SPI_HandleTypeDef *spi3)
{
 r4_result.stage=0;r4_result.core_hz=HAL_RCC_GetHCLKFreq();
 r4_result.spi1_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI1)/64u;
 r4_result.spi3_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3)/4u;
 if(!identity_ok()||r4_result.core_hz!=160000000u||r4_result.spi1_hz!=2500000u||
    r4_result.spi3_hz!=4000000u||spi1->Init.BaudRatePrescaler!=SPI_BAUDRATEPRESCALER_64||
    stm32_fram_storage_init(&fram,spi1,GPIOD,GPIO_PIN_14,2)!=TR2_OK||
    stm32_live_acquisition_attach(&owner,spi3,5,consume,NULL)!=TR2_OK) {
  r4_result.error=TR2_ERROR_UNAVAILABLE;for(;;)__WFI();
 }
 /* ST IIS3DWB datasheet Ton=10ms typical, no guaranteed maximum.
  * 20ms is a bounded experimental startup allowance, never a claimed WCET.
  * Service gap remains5ms and status/overflow are checked on every poll. */
 owner.acquisition.fifo.first_data_timeout_ms=20;
 previous_cycles=DWT->CYCCNT;elapsed_cycles=0;
 bounded=stm32_fram_storage_bounded_physical(&fram);
 if(tr2_cooperative_init(&work,NULL,now_us,service,stop,32,1000,2500)!=TR2_OK)for(;;)__WFI();
 r4_result.stage=1;
 while(r4_trigger!=0x52340001u&&r4_trigger!=0x52340002u){(void)now_us(NULL);HAL_Delay(1);}
 uint32_t mode=r4_trigger&0xFFFFu;r4_trigger=0;r4_result.command=mode;
 r4_result.tick_start=HAL_GetTick();Tr2Result status=qualify(mode);
 if(status!=TR2_OK){if(!r4_result.error)r4_result.error=status;stop(NULL);r4_result.stage|=0x80000000u;}
 else r4_result.stage=10;
 snapshot();r4_result.ownership=0;
 for(;;){(void)now_us(NULL);HAL_Delay(1);}
}
