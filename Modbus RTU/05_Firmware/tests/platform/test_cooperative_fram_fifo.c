/* R2's real stores/codecs/fixtures, not its regression main. */
#define TR2_LIVE_R2_TEST_LIBRARY 1
#include "../unit/test_live_command_engine.c"
#include "live_fifo_acquisition.h"
#include "tr2/modbus/rtu_server_runtime.h"

typedef struct {
 Bench *b;LiveFifoAcquisition acquisition;Tr2CooperativeWork work;
 uint8_t regs[128];uint64_t us,remainder,total_words,last_fifo_us,max_fifo_gap;
 uint32_t level,max_level,overrun,word_index,xyz,ts;
 size_t max_io;unsigned polls,safety,rtu_replies;bool reentry_checked,emergency,uncapped,deny_io;
 uint64_t emergency_at,stopped_at,request_at,max_rtu_latency,next_request;
 unsigned serial_index,request_bytes,max_rx_backlog;uint8_t request[256];ModbusRtuServerRuntime rtu;SerialTransport transport;
 ModbusBlock0Image b0;
} Model;
static void advance_model(Model *m,uint64_t us)
{
 m->us+=us;
 if(m->regs[16]&0xE0u) {
  m->remainder+=us*240003u;uint32_t n=(uint32_t)(m->remainder/8000000u);m->remainder%=8000000u;
  m->level+=n;m->total_words+=n;if(m->level>m->max_level)m->max_level=m->level;
  if(m->level>=512)m->overrun++;
 }
}
static uint8_t fifo_tag(unsigned kind,unsigned count)
{uint8_t v=(uint8_t)((kind<<3)|(count<<1));return v|(__builtin_parity(v)&1);}
static int32_t sensor_read(void *ctx,uint8_t reg,uint8_t *b,uint16_t n)
{
 Model *m=ctx;advance_model(m,10u+2u*n); /* explicit simulated SPI4MHz+10us */
 if(reg==58){b[0]=(uint8_t)m->level;b[1]=(uint8_t)(m->level>>8)|(m->overrun?0x60:0);return 0;}
 if(reg==120) {
  assert(n%7u==0&&n/7u<=m->level);m->level-=n/7u;
  for(unsigned i=0;i<n;i+=7) {
   memset(b+i,0,7);
   if(m->word_index++%9u==0) {b[i]=fifo_tag(4,0);m->ts+=24;memcpy(b+i+1,&m->ts,4);}
   else {b[i]=fifo_tag(2,m->xyz%4);uint16_t value=m->xyz++%1000u;memcpy(b+i+1,&value,2);}
  }
  return 0;
 }
 assert(reg+n<=128);memcpy(b,m->regs+reg,n);return 0;
}
static int32_t sensor_write(void *ctx,uint8_t reg,const uint8_t *b,uint16_t n)
{Model *m=ctx;advance_model(m,10u+2u*n);assert(reg+n<=128);memcpy(m->regs+reg,b,n);return 0;}
static int consume_raw(void *ctx,const uint8_t *words,uint32_t n,uint64_t index)
{(void)ctx;(void)words;(void)n;(void)index;return 0;}
static uint64_t model_ms(void *ctx){return ((Model *)ctx)->us/1000u;}
static uint64_t model_us(void *ctx)
{
 Model *m=ctx;advance_model(m,4); /* declared CPU allowance per checkpoint boundary */
 return m->us;
}
static void safety(void *ctx)
{Model *m=ctx;m->safety++;(void)fifo_live_stop(&m->acquisition.fifo);m->stopped_at=m->us;}
static Tr2Result physical_read_model(void *ctx,uint32_t at,void *b,size_t n)
{
 Model *m=ctx;assert(m->uncapped||n<=m->work.quantum_bytes);if(n>m->max_io)m->max_io=n;
 advance_model(m,20u+(uint64_t)(n+4u)*32u/10u); /* SPI1 2.5MHz +20us */
 return rd(&m->b->f->physical,at,b,n);
}
static Tr2Result physical_write_model(void *ctx,uint32_t at,const void *b,size_t n)
{
 Model *m=ctx;assert(m->uncapped||n<=m->work.quantum_bytes);if(n>m->max_io)m->max_io=n;
 advance_model(m,40u+(uint64_t)(n+8u)*32u/10u); /* WREN/RDSR/WRITE/WRDI allowance */
 return wr(&m->b->f->physical,at,b,n);
}
static Tr2Result serial_start_model(void *ctx){(void)ctx;return TR2_OK;}
static void next_frame(Model *m,bool large)
{
 uint8_t pdu[252]={16,0,0,0,123,246};size_t bytes;
 if(!large){pdu[0]=3;pdu[3]=0;pdu[4]=21;}
 assert(modbus_rtu_adu_encode(17,pdu,large?252:5,m->request,sizeof(m->request),&bytes)==MODBUS_RTU_CODEC_OK);
 m->request_bytes=(unsigned)bytes;
}
static Tr2Result serial_tx_model(void *ctx,const uint8_t *data,size_t n)
{
 Model *m=ctx;ModbusRtuAduView v;
 assert(modbus_rtu_adu_decode(data,n,&v)==MODBUS_RTU_CODEC_OK&&v.unit_id==17);
 if(m->request_bytes==8)assert(v.pdu[0]==3&&v.pdu_length==44);
 else assert(v.pdu_length==2&&v.pdu[0]==0x90&&v.pdu[1]==2); /* FC10 crosses RO/absent addresses */
 uint64_t latency=m->us-m->request_at;if(latency>m->max_rtu_latency)m->max_rtu_latency=latency;
 m->rtu_replies++;m->next_request=m->us+n*96u+350u;m->serial_index=0;next_frame(m,m->rtu_replies%2u!=0);advance_model(m,20);return TR2_OK;
}
static bool serial_poll_model(void *ctx,SerialTransportEvent *event)
{
 Model *m=ctx;*event=(SerialTransportEvent){SERIAL_TRANSPORT_EVENT_NONE,0,SERIAL_TRANSPORT_ERROR_UNSPECIFIED};
 if(m->us<m->next_request)return true;
 if(!m->serial_index)m->request_at=m->us;
 uint64_t elapsed=m->us-m->request_at;
 unsigned arrived=(unsigned)(elapsed/96u)+1u;if(arrived>m->request_bytes)arrived=m->request_bytes;
 if(m->serial_index<m->request_bytes&&arrived-m->serial_index>m->max_rx_backlog)m->max_rx_backlog=arrived-m->serial_index;
 assert(m->max_rx_backlog<64u); /* same event queue capacity as STM32 */
 if(m->serial_index<m->request_bytes&&elapsed>=m->serial_index*96u) {
  event->type=SERIAL_TRANSPORT_EVENT_BYTE;event->byte=m->request[m->serial_index++];
 } else if(m->serial_index==m->request_bytes&&elapsed>=(m->request_bytes-1u)*96u+150u) {
  event->type=SERIAL_TRANSPORT_EVENT_SILENCE_T1_5;m->serial_index++;
 } else if(m->serial_index==m->request_bytes+1u&&elapsed>=(m->request_bytes-1u)*96u+350u) {
  event->type=SERIAL_TRANSPORT_EVENT_SILENCE_T3_5;m->serial_index++;
 }
 return true;
}
static Tr2Result cooperative_service(void *ctx)
{
 Model *m=ctx;
 if(!m->reentry_checked) {
  size_t writes=m->b->f->physical.writes;
  PersistentMedia *media=transactional_image_media_interface(&m->b->f->live);
  assert(media->commit(media->context)==TR2_ERROR_INVALID_STATE);
  LiveCommandRequest q=request(1,200,13);
  assert(live_command_engine_submit(&m->b->engine,&q)==TR2_ERROR_INVALID_STATE);
  assert(m->b->f->physical.writes==writes);m->reentry_checked=true;
 }
 if(m->emergency&&m->us>=m->emergency_at)return TR2_ERROR_UNAVAILABLE;
 if(m->acquisition.fifo.running) {
  if(m->last_fifo_us&&m->us-m->last_fifo_us>m->max_fifo_gap)m->max_fifo_gap=m->us-m->last_fifo_us;
  m->last_fifo_us=m->us;m->polls++;
  if(live_fifo_acquisition_poll(&m->acquisition)!=TR2_OK)return TR2_ERROR_UNAVAILABLE;
 }
 for(unsigned i=0;i<8;i++) {
  advance_model(m,2);Tr2Result r=modbus_rtu_server_runtime_poll_once(&m->rtu);if(r!=TR2_OK)return r;
 }
 return TR2_OK;
}
static bool bounded_admission(void *ctx,uint16_t code)
{
 Model *m=ctx;(void)code;
 if(m->deny_io||tr2_cooperative_status(&m->work)!=TR2_OK)return false;
 uint64_t reserve=((uint64_t)m->work.work_limit_us+m->work.service_limit_us)*240003u;
 reserve=(reserve+7999999u)/8000000u;
 return reserve+m->acquisition.fifo.parser.last_level<512u;
}
static Model *model_new(uint16_t quantum)
{
 Model *m=calloc(1,sizeof(*m));assert(m);m->b=bench();
 m->regs[15]=0x7B;m->regs[18]=4;
 stmdev_ctx_t ctx={0};ctx.handle=m;ctx.read_reg=sensor_read;ctx.write_reg=sensor_write;
 assert(live_fifo_acquisition_init(&m->acquisition,&ctx,model_ms,m,5,consume_raw,NULL)==TR2_OK);
 LiveAcquisitionPort port=live_fifo_acquisition_port(&m->acquisition);
 /* Explicit model admission only; physical production has no such grant. */
 m->acquisition.budget=bounded_admission;m->acquisition.budget_context=m;
 assert(live_command_engine_init(&m->b->engine,&m->b->store,&port)==TR2_OK);
 m->transport=(SerialTransport){m,serial_start_model,serial_tx_model,serial_poll_model};
 m->b0.registers[6]=1;ModbusPduServerContext pdu={0};pdu.read_sources.b0_image=&m->b0;
 assert(modbus_rtu_server_runtime_init(&m->rtu,&m->transport,17,&pdu)==TR2_OK);
 assert(modbus_rtu_server_runtime_start(&m->rtu)==TR2_OK);
 next_frame(m,false);
 assert(tr2_cooperative_init(&m->work,m,model_us,cooperative_service,safety,quantum,1000,2500)==TR2_OK);
 TransactionalImagePhysicalStorage storage={m,physical_read_model,physical_write_model};
 m->b->f->historical.physical=m->b->f->live.physical=storage;
 assert(transactional_image_media_bind_work(&m->b->f->historical,&m->work)==TR2_OK);
 assert(transactional_image_media_bind_work(&m->b->f->live,&m->work)==TR2_OK);
 assert(live_command_engine_boot(&m->b->engine)==TR2_OK);
 return m;
}
static void model_free(Model *m){destroy(m->b);free(m);}
static void sequence_run(Bench *b)
{
 LiveCommandRequest q=request(1,1,13);run_command(b,&q);
 q=request(1,65535,12);run_command(b,&q);q=request(2,2,14);run_command(b,&q);
 PreparedConfiguration p=prepared(vector_2_prepared,3);assert(live_command_engine_prepare(&b->engine,&p)==TR2_OK);
 q=request(2,3,1);run_command(b,&q);
}
static void format_and_load(void)
{
 Bench *reference=bench();sequence_run(reference);
 for(unsigned quantum=16;quantum<=64;quantum*=2) {
  Model *m=model_new((uint16_t)quantum);sequence_run(m->b);
  assert(!memcmp(reference->f->physical.bytes,m->b->f->physical.bytes,262144));
  assert(!m->overrun&&m->rtu_replies>100&&m->max_io==quantum&&m->acquisition.fifo.total_xyz>100000);
  assert(!m->acquisition.metrology); /* absent calibrated time -> no B3 forged */
  printf("COOPERATIVE,q=%u,elapsed_us=%llu,work_us=%llu,service_us=%llu,FIFOmax=%u,gap_us=%llu,XYZ=%llu,RTU=%u,RTUenqueue_us=%llu,identical_bytes=262144\n",
   quantum,(unsigned long long)m->us,(unsigned long long)m->work.max_work_us,(unsigned long long)m->work.max_service_us,
   m->max_level,(unsigned long long)m->max_fifo_gap,(unsigned long long)m->acquisition.fifo.total_xyz,m->rtu_replies,(unsigned long long)m->max_rtu_latency);
  if(quantum==32) {
   LiveCommandRequest q=request(2,4,13);run_command(m->b,&q);unsigned renews=0;
   while(m->us<600000000u) {
    q=request(m->b->store.epoch.current,65535,12);run_command(m->b,&q);assert(++renews<100);
   }
   assert(!m->overrun&&m->acquisition.fifo.total_xyz>10000000u);
   printf("LONG_MODEL,elapsed_us=%llu,XYZ=%llu,renewals=%u,FIFOmax=%u,RTU=%u,RXqueueMax=%u,overflow=0\n",
    (unsigned long long)m->us,(unsigned long long)m->acquisition.fifo.total_xyz,renews,m->max_level,m->rtu_replies,m->max_rx_backlog);
   q=request(m->b->store.epoch.current,1,14);run_command(m->b,&q);assert(!m->acquisition.fifo.running);
  }
  model_free(m);
 }
 destroy(reference);
}
static void negative_control(void)
{
 Model *m=model_new(32);LiveCommandRequest q=request(1,1,13);run_command(m->b,&q);
 m->b->f->historical.work=m->b->f->live.work=NULL;m->uncapped=true;
 q=request(1,65535,12);assert(live_command_engine_submit(&m->b->engine,&q)==TR2_OK);
 assert(m->overrun&&live_fifo_acquisition_poll(&m->acquisition)!=TR2_OK);
 puts("NEGATIVE CONTROL: same actual stores without cooperative service overflow the independent FIFO producer: PASS");
 model_free(m);
}
static void emergency_and_fault(void)
{
 Model *m=model_new(32);LiveCommandRequest q=request(1,1,13);
 size_t initial_writes=m->b->f->physical.writes;m->deny_io=true;
 assert(live_command_engine_submit(&m->b->engine,&q)==TR2_OK&&m->b->engine.result==7);
 assert(m->b->f->physical.writes==initial_writes);m->deny_io=false;run_command(m->b,&q);
 m->emergency=true;m->emergency_at=m->us+500;
 q=request(1,65535,12);assert(live_command_engine_submit(&m->b->engine,&q)!=TR2_OK);
 assert(m->safety&&!m->acquisition.fifo.running&&!m->work.depth);
 assert(m->stopped_at-m->emergency_at<1000u&&!m->overrun);
 size_t writes=m->b->f->physical.writes;
 assert(live_command_engine_submit(&m->b->engine,&q)!=TR2_OK&&m->b->f->physical.writes==writes);
 printf("EMERGENCY,latency_us=%llu,sticky_error=%u,no_reentry_no_write=PASS\n",(unsigned long long)(m->stopped_at-m->emergency_at),m->work.error);
 model_free(m);
 m=model_new(32);q=request(1,1,13);run_command(m->b,&q);m->b->f->physical.budget=0;q=request(1,2,14);
 assert(live_command_engine_submit(&m->b->engine,&q)!=TR2_OK&&!m->acquisition.fifo.running);model_free(m);
 puts("FRAM failure stops actual FIFO owner independently of journal success: PASS");
}
int tr2_cooperative_model_regression(void){format_and_load();emergency_and_fault();negative_control();return 0;}
#ifndef TR2_COOPERATIVE_MODEL_LIBRARY
int main(void){return tr2_cooperative_model_regression();}
#endif
