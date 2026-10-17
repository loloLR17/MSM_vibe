/* Reuse R4's independently timed FIFO/SPI/FRAM model and real R1/R2 fixtures;
 * do not call its regression suite here. All clocks below are SIMULATED. */
#define TR2_COOPERATIVE_MODEL_LIBRARY 1
#include "test_cooperative_fram_fifo.c"
#include "tr2/application/live_modbus_server.h"

static LiveModbusServer server;
static LiveMetrology metric;
static uint64_t origin;
static bool utc_available;
static bool inside_rtu;
static uint32_t b3_responses,b5_responses;
static int32_t timed_write(void *context,uint8_t reg,const uint8_t *bytes,uint16_t n)
{
 Model *m=context;bool was=(m->regs[16]&0xE0)!=0;
 int32_t r=sensor_write(context,reg,bytes,n);
 if(!was&&(m->regs[16]&0xE0))origin=m->us;
 return r;
}
static Tr2Result sample_time_model(void *context,uint64_t index,uint64_t *mono,bool *valid,uint64_t *utc)
{
 (void)context;*mono=(origin+index*1000000u/26667u)/1000u;
 *valid=utc_available;*utc=1000u+*mono/1000u;return TR2_OK;
}
static void load_read(Model *m,unsigned a,unsigned n)
{
 uint8_t pdu[5]={3,(uint8_t)(a>>8),(uint8_t)a,(uint8_t)(n>>8),(uint8_t)n};size_t bytes;
 assert(modbus_rtu_adu_encode(17,pdu,5,m->request,sizeof(m->request),&bytes)==MODBUS_RTU_CODEC_OK);
 m->request_bytes=(unsigned)bytes;
}
static Tr2Result tx_snapshot_model(void *context,const uint8_t *bytes,size_t n)
{
 Model *m=context;ModbusRtuAduView v;
 assert(modbus_rtu_adu_decode(bytes,n,&v)==MODBUS_RTU_CODEC_OK&&v.unit_id==17&&v.pdu[0]==3);
 unsigned address=(unsigned)m->request[2]*256u+m->request[3];
 if(address==3000){assert(v.pdu_length==98);b3_responses++;}
 else if(address==5000){assert(v.pdu_length==60);b5_responses++;}
 else assert(address==0&&v.pdu_length==44);
 uint64_t latency=m->us-m->request_at;if(latency>m->max_rtu_latency)m->max_rtu_latency=latency;
 m->rtu_replies++;m->next_request=m->us+n*96u+350u;m->serial_index=0;
 load_read(m,m->rtu_replies%3u==0?5000:3000,m->rtu_replies%3u==0?29:48);
 advance_model(m,20);return TR2_OK;
}
static Tr2Result live_service(void *context)
{
 Model *m=context;
 size_t writes=m->b->f->physical.writes;
 assert(live_modbus_server_owner_step(&server)==TR2_ERROR_INVALID_STATE);
 assert(m->b->f->physical.writes==writes); /* no R2 recursion in a quantum */
 if(m->acquisition.fifo.running) {
  if(m->last_fifo_us&&m->us-m->last_fifo_us>m->max_fifo_gap)m->max_fifo_gap=m->us-m->last_fifo_us;
  m->last_fifo_us=m->us;m->polls++;
  uint64_t before=m->acquisition.fifo.total_xyz;
  if(live_fifo_acquisition_poll(&m->acquisition)!=TR2_OK)return TR2_ERROR_UNAVAILABLE;
  advance_model(m,12u+5u*(m->acquisition.fifo.total_xyz-before)); /* declared CPU allowance */
 }
 Tr2Result r=live_modbus_server_publish(&server,m->us/1000u);if(r!=TR2_OK)return r;
 for(unsigned i=0;i<8;i++) {
  size_t reads=m->b->f->physical.read_bytes,written=m->b->f->physical.written;
  uint32_t hardware=m->polls;inside_rtu=true;
  r=live_modbus_server_poll_once(&server);inside_rtu=false;
  assert(m->b->f->physical.read_bytes==reads&&m->b->f->physical.written==written&&hardware==m->polls);
  if(r!=TR2_OK)return r;
  advance_model(m,4);
 }
 return TR2_OK;
}
static Model *live_model(void)
{
 Model *m=model_new(32);live_metrology_init(&metric);origin=0;utc_available=false;b3_responses=b5_responses=0;
 m->acquisition.fifo.ctx.write_reg=timed_write;
 assert(live_fifo_acquisition_bind_metrology(&m->acquisition,&metric,sample_time_model,m)==TR2_OK);
 /* model_new already booted the engine; bind observer through a fresh boot */
 LiveAcquisitionPort port=live_fifo_acquisition_port(&m->acquisition);
 assert(live_command_engine_init(&m->b->engine,&m->b->store,&port)==TR2_OK);
 assert(live_fifo_acquisition_bind_engine(&m->acquisition,&m->b->engine)==TR2_OK);
 m->b0.registers[6]=2;m->b0.registers[7]=12; /* prepared identity, no false LIVE bit */
 m->transport.transmit=tx_snapshot_model;
 assert(live_modbus_server_init(&server,&m->b->engine,&metric,&m->transport,17,&m->b0,model_ms,m)==TR2_OK);
 m->serial_index=0;m->next_request=m->us;load_read(m,3000,48);
 m->work.service=live_service;
 assert(live_command_engine_boot(&m->b->engine)==TR2_OK);
 assert(live_modbus_server_publish(&server,m->us/1000u)==TR2_OK);
 return m;
}
static ModbusPduServerOutcome pdu(Model *m,const uint8_t *q,size_t n,uint8_t *out)
{
 size_t reads=m->b->f->physical.read_bytes,writes=m->b->f->physical.written;
 uint32_t io=m->polls;inside_rtu=true;
 ModbusPduServerOutcome r=live_modbus_server_process_pdu(&server,q,n,out,253);inside_rtu=false;
 assert(m->b->f->physical.read_bytes==reads&&m->b->f->physical.written==writes&&m->polls==io);return r;
}
static void write_words(Model *m,unsigned at,const uint16_t *v,unsigned n,unsigned expected)
{
 uint8_t q[253]={16,(uint8_t)(at>>8),(uint8_t)at,0,(uint8_t)n,(uint8_t)(2*n)},out[253];
 for(unsigned i=0;i<n;i++){q[6+2*i]=(uint8_t)(v[i]>>8);q[7+2*i]=(uint8_t)v[i];}
 ModbusPduServerOutcome r=pdu(m,q,6+2*n,out);assert(r.operation_result==TR2_OK);
 assert(expected?(out[0]==0x90&&out[1]==expected&&r.response_length==2):(out[0]==16&&r.response_length==5));
}
static void capture(Model *m,unsigned epoch,unsigned tx,unsigned code,unsigned expected)
{
 uint16_t e[2]={(uint16_t)(epoch>>16),(uint16_t)epoch},v[8]={(uint16_t)code,(uint16_t)tx,0,0,0,0,0,1};
 write_words(m,5020,e,2,0);write_words(m,5000,v,8,expected);
}
static void drain(Model *m)
{
 for(unsigned i=0;i<40;i++) {
  assert(live_modbus_server_owner_step(&server)==TR2_OK);
  bool pending=false;for(unsigned j=0;j<4;j++)pending|=server.queue[j].used;
  if(!pending&&!m->b->engine.has_active)return;
 }
 assert(false);
}
static void lifecycle(void)
{
 Model *m=live_model();uint8_t out[253];uint8_t read[5]={3,11,184,0,48};
 assert(pdu(m,read,5,out).response_length==98&&out[3]==1&&!(out[5]&6u)); /* completed boot: inactive B3, V/F=0 */
 size_t writes=m->b->f->physical.writes;capture(m,1,1,13,0);
 assert(m->b->f->physical.writes==writes&&!m->b->engine.has_active);
 drain(m);assert(m->b->engine.status==4&&metric.running&&metric.sequence>0);
 assert(b3_responses>10&&b5_responses>0&&!m->overrun);
 assert(live_modbus_server_publish(&server,m->us/1000u)==TR2_OK);
 const LiveModbusImage *image=&server.images[server.published];
 assert((image->b3.registers[1]&6u)==6u&&!(image->b3.registers[1]&64u)); /* missing UTC visible */
 assert(image->run_id==1&&image->config_id==2&&image->config_generation==2);
 /* Publication alone never changes values; response is a complete pinned image. */
 for(unsigned i=0;i<200;i++) {
  ModbusBlock3Image expected=server.images[server.published].b3;
  assert(pdu(m,read,5,out).response_length==98);
  for(unsigned j=0;j<48;j++)assert(((unsigned)out[2+2*j]*256u+out[3+2*j])==expected.registers[j]);
  assert(live_modbus_server_publish(&server,m->us/1000u)==TR2_OK);
 }
 /* Lost ACK/retry: captured twice, same durable transaction, no second effect. */
 uint64_t run=m->b->store.effect.run_id;writes=m->b->f->physical.writes;
 capture(m,1,1,13,0);drain(m);assert(m->b->store.effect.run_id==run&&m->b->f->physical.writes==writes);
 capture(m,1,1,14,0);drain(m);assert(m->b->engine.result==27&&metric.running);
 /* Saturate normal captures, reserve STOP; no failed capture mutates mailbox. */
 capture(m,1,20,1,0);capture(m,1,21,1,0);capture(m,1,22,1,0);
 uint16_t saved[8];memcpy(saved,server.mailbox,sizeof(saved));capture(m,1,23,1,4);
 assert(!memcmp(saved,server.mailbox,sizeof(saved))&&server.queue_full==1);
 capture(m,1,2,14,0);assert(server.queue[3].used);
 assert(live_modbus_server_owner_step(&server)==TR2_OK&&m->b->engine.active.request.command.identity.command_code==14);
 /* Following ordinary captures are refused against nonterminal, not retried. */
 drain(m);assert(!metric.running&&!m->acquisition.fifo.running&&server.captured==7);
 assert(live_modbus_server_publish(&server,m->us/1000u)==TR2_OK);
 assert(!(server.images[server.published].b3.registers[1]&4u));
 assert(!m->work.deadline_missed&&m->work.max_work_us<=1000&&m->work.max_service_us<=2500&&m->max_fifo_gap<=5000);
 printf("R4Q_MODEL,work_us=%llu,service_us=%llu,gap_us=%llu,FIFOmax=%u,XYZ=%llu,RTU=%u,B3=%u,B5=%u,RXmax=%u,response_enqueue_us=%llu,sequence=%u,STOP_reserved=PASS\n",
 (unsigned long long)m->work.max_work_us,(unsigned long long)m->work.max_service_us,(unsigned long long)m->max_fifo_gap,m->max_level,
 (unsigned long long)m->acquisition.fifo.total_xyz,m->rtu_replies,b3_responses,b5_responses,m->max_rx_backlog,(unsigned long long)m->max_rtu_latency,metric.sequence);
 model_free(m);
}
typedef struct {
 SerialTransportEvent events[270];unsigned count,index,transmits;
 uint8_t response[256];size_t length;bool disconnect,tx_fail;
} Wire;
static Tr2Result wire_start(void *context){(void)context;return TR2_OK;}
static bool wire_poll(void *context,SerialTransportEvent *out)
{
 Wire *w=context;if(w->disconnect)return false;
 *out=w->index<w->count?w->events[w->index++]:(SerialTransportEvent){SERIAL_TRANSPORT_EVENT_NONE,0,0};return true;
}
static Tr2Result wire_tx(void *context,const uint8_t *bytes,size_t n)
{
 Wire *w=context;if(w->tx_fail)return TR2_ERROR_UNAVAILABLE;
 assert(n<=sizeof(w->response));memcpy(w->response,bytes,n);w->length=n;w->transmits++;return TR2_OK;
}
static void wire_load(Wire *w,const uint8_t *bytes,size_t n,bool overrun)
{
 w->count=w->index=0;
 for(unsigned i=0;i<n;i++)w->events[w->count++]=(SerialTransportEvent){SERIAL_TRANSPORT_EVENT_BYTE,bytes[i],0};
 if(overrun)w->events[w->count++]=(SerialTransportEvent){SERIAL_TRANSPORT_EVENT_ERROR,0,SERIAL_TRANSPORT_ERROR_OVERRUN};
 w->events[w->count++]=(SerialTransportEvent){SERIAL_TRANSPORT_EVENT_SILENCE_T3_5,0,0};
}
static void transport_and_access(void)
{
 Model *m=live_model();Wire w={0};SerialTransport t={&w,wire_start,wire_tx,wire_poll};
 server.rtu.transport=&t;assert(modbus_rtu_server_runtime_start(&server.rtu)==TR2_OK);
 uint8_t q[]={3,11,184,0,48},adu[256];size_t n;
 assert(modbus_rtu_adu_encode(17,q,5,adu,sizeof(adu),&n)==MODBUS_RTU_CODEC_OK);
 size_t reads=m->b->f->physical.read_bytes,writes=m->b->f->physical.written;
 for(unsigned bad=0;bad<3;bad++) {
  if(bad==1)adu[n-1]^=1;
  wire_load(&w,adu,n,bad==2);
  while(w.index<w.count)assert(live_modbus_server_poll_once(&server)==TR2_OK);
  assert(w.transmits==(bad==0?1u:1u));
  if(bad==1)adu[n-1]^=1;
 }
 /* Receiver overflow (>256 bytes) drops the whole frame; following frame works. */
 uint8_t oversized[257]={0};wire_load(&w,oversized,sizeof(oversized),false);
 while(w.index<w.count)assert(live_modbus_server_poll_once(&server)==TR2_OK);
 assert(w.transmits==1);
 wire_load(&w,adu,n,false);while(w.index<w.count)assert(live_modbus_server_poll_once(&server)==TR2_OK);
 assert(w.transmits==2);w.disconnect=true;assert(live_modbus_server_poll_once(&server)!=TR2_OK);w.disconnect=false;
 wire_load(&w,adu,n,false);while(w.index<w.count)assert(live_modbus_server_poll_once(&server)==TR2_OK);
 assert(w.transmits==3);w.tx_fail=true;wire_load(&w,adu,n,false);bool failed=false;
 while(w.index<w.count)if(live_modbus_server_poll_once(&server)!=TR2_OK)failed=true;
 assert(failed&&server.transport_errors==2);
 assert(m->b->f->physical.read_bytes==reads&&m->b->f->physical.written==writes);
 uint16_t zero[2]={0,0};write_words(m,5007,zero,1,0);write_words(m,5007,zero,2,2);
 write_words(m,5021,zero,1,0);write_words(m,5028,zero,1,2);
 uint8_t out[253],invalid[]={3,19,165,0,2};assert(pdu(m,invalid,5,out).response_length==2&&out[1]==2);
 ModbusBlock0Image b0=server.identity;b0.registers[6]=1;
 LiveModbusServer reject;assert(live_modbus_server_init(&reject,&m->b->engine,&metric,&t,17,&b0,model_ms,m)!=TR2_OK);
 model_free(m);puts("RTU CRC/address/framing overflow/UART overrun/disconnect/TX failure and V1 profile isolation: PASS");
}
static void read_time_quality(void)
{
 Model *m=live_model();ActiveConfigurationSnapshot c=m->b->f->service.active;
 assert(live_metrology_start(&metric,1,&c)==TR2_OK);int16_t xyz[3]={100,20,-30};
 uint64_t start=m->us/1000u;
 for(unsigned i=0;i<c.payload.window_size_samples;i++)assert(live_metrology_sample(&metric,1,c.config_id,c.generation,i,xyz,start+i/26u,true,123)==TR2_OK);
 m->us=(metric.last_ms+1)*1000u;assert(live_modbus_server_publish(&server,m->us/1000u)==TR2_OK);
 uint8_t q[]={3,11,184,0,48},out[253];assert(pdu(m,q,5,out).response_length==98&&(out[5]&6u)==6u);
 m->us=(metric.value.value_monotonic_ms+2u*c.payload.indicator_period_ms+1u)*1000u;
 assert(pdu(m,q,5,out).response_length==98&&!(out[5]&4u)&&(out[4]&2u)); /* HELD=bit9 */
 assert(metric.value_valid&&metric.running); /* private RAM projection, no producer mutation */
 m->us-=1000u;assert(pdu(m,q,5,out).response_length==98&&out[3]==5&&!(out[5]&6u));
 assert(server.time_fault&&metric.running);assert(live_modbus_server_owner_step(&server)==TR2_OK);
 assert(!metric.running&&!m->acquisition.fifo.running&&m->b->engine.blocking_fault);
 model_free(m);puts("Read-time age/freshness on immutable RAM, UTC retained and backward clock invalidation deferred to owner: PASS");
}
static void prepared_apply_and_recovery(void)
{
 Model *m=live_model();uint16_t id[2]={0,3};write_words(m,4002,id,2,0);
 PreparedConfiguration prep=prepared(vector_2_prepared,3);
 uint16_t crc[2]={(uint16_t)(prep.supplied_crc>>16),(uint16_t)prep.supplied_crc};write_words(m,4008,crc,2,0);
 for(unsigned i=0;i<84;i++) {
  const ModbusRegisterDescriptor *d=modbus_register_model_find((uint16_t)(4016+i));
  if(d&&d->access==MODBUS_REGISTER_RW)write_words(m,4016+i,vector_2_prepared+i,1,0);
 }
 assert(configuration_staging_snapshot(&server.staging,&prep));
 assert(prep.config_id==3&&prep.supplied_crc==tr2_b4_prepared_payload_crc(&prep.payload));
 size_t writes=m->b->f->physical.writes;capture(m,1,10,1,0);
 uint16_t replacement[2]={0,99};write_words(m,4002,replacement,2,0);
 assert(m->b->f->physical.writes==writes);drain(m);
 assert(m->b->engine.status==4&&m->b->f->service.active.config_id==3&&m->b->f->service.active.generation==3);
 assert(server.staging.prepared.config_id==99); /* immutable capture used config3 */
 assert(live_modbus_server_publish(&server,m->us/1000u)==TR2_OK);
 uint8_t read[5]={3,15,162,0,4},out[253]; /*4002prepared /4004active */
 assert(pdu(m,read,5,out).response_length==10&&out[5]==99&&out[9]==3);
 capture(m,1,11,13,0);drain(m);assert(m->b->engine.status==4&&m->acquisition.fifo.run_id==1);
 uint64_t run=m->b->store.effect.run_id;writes=m->b->f->physical.writes;
 /* Reboot loses ingress/snapshot, preserves durable receipts, no auto-start. */
 assert(live_command_engine_boot(&m->b->engine)==TR2_OK&&!m->acquisition.fifo.running);
 live_metrology_init(&metric);
 assert(live_modbus_server_init(&server,&m->b->engine,&metric,&m->transport,17,&m->b0,model_ms,m)==TR2_OK);
 assert(live_modbus_server_publish(&server,m->us/1000u)==TR2_OK);
 capture(m,1,11,13,0);drain(m);
 assert(m->b->engine.status==4&&!m->acquisition.fifo.running&&m->b->store.effect.run_id==run&&m->b->f->physical.writes==writes);
 /* Fault in admission: no successful result fabricated, independent stop. */
 m->b->f->physical.budget=0;capture(m,1,12,13,0);
 assert(live_modbus_server_owner_step(&server)!=TR2_OK);
 assert(m->b->engine.blocking_fault&&!m->acquisition.fifo.running&&!metric.value_valid);
 model_free(m);puts("B4 RAM staging -> immutable APPLY -> durable START; lost ACK/reboot retry no effect; FRAM fault invalidation: PASS");
}
static void four_windows(void)
{
 const unsigned windows[]={4096,8192,16384,32768};
 for(unsigned w=0;w<4;w++) {
  Model *m=live_model();ActiveConfigurationSnapshot c=m->b->f->service.active;
  c.payload.window_size_samples=(uint16_t)windows[w];c.payload.full_scale_code=0;c.payload.axes_enable_mask=1;
  assert(live_metrology_start(&metric,1,&c)==TR2_OK);uint64_t begin=m->us/1000u;
  int16_t xyz[3]={1000,0,0};
  for(unsigned i=0;i<windows[w];i++)assert(live_metrology_sample(&metric,1,c.config_id,c.generation,i,xyz,begin+(uint64_t)i*1000u/26667u,w%2==0,1234)==TR2_OK);
  m->us=metric.last_ms*1000u;assert(live_modbus_server_publish(&server,m->us/1000u)==TR2_OK);
  uint8_t q[]={3,11,184,0,48},out[253];assert(pdu(m,q,5,out).response_length==98);
  assert(out[2+13*2]==(uint8_t)(windows[w]>>8)&&out[3+13*2]==(uint8_t)windows[w]);
  assert(out[2+15*2]==0&&out[3+15*2]==61); /* independent constant:1000*61ug=61mg */
  assert(out[2+17*2]==0&&out[3+17*2]==61);
  assert((out[5]&6u)==6u&&!!(out[5]&64u)==(w%2==0));
  if(w%2)assert(out[10]==0&&out[11]==0&&out[12]==0&&out[13]==0); /* UTC unavailable */
  assert(server.images[server.published].run_id==1&&server.images[server.published].config_generation==c.generation);
  model_free(m);
 }
 puts("Four N=4096/8192/16384/32768 windows through B3 PDU;61mg oracle, identity and UTC valid/unavailable: PASS");
}
int tr2_live_modbus_regression(void){lifecycle();transport_and_access();read_time_quality();prepared_apply_and_recovery();four_windows();puts("R4Q actual FIFO -> R3B -> coherent RAM -> RTU and deferred durable R2: PASS (SIMULATION)");return 0;}

#ifndef TR2_LIVE_MODBUS_MODEL_LIBRARY
int main(void){return tr2_live_modbus_regression();}
#endif
