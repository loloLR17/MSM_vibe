#include "sd_writer.h"
#include "stream_hash.h"
#include "capture_queue.h"
#include "tr2/common/sha256.h"
#include "fifo_event_time.h"
#include "tr2/domain/acquisition/fir_decimator10.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint8_t q[2048],memory[128][512];static unsigned used,pos,writing,crcbytes;
static bool selected,initialized,crc_bad,no_token,no_response,busy_forever,io_fail,stuck_after_data;
static uint64_t clock_us;static uint32_t pending_lba,write_offset;static uint64_t inject_busy_us,busy_until;static uint16_t actual_write_crc;
static void append(uint8_t b){assert(used<sizeof q);q[used++]=b;}
static void block(const uint8_t *b,unsigned n){append(0xfe);for(unsigned i=0;i<n;i++)append(b[i]);uint16_t c=sd_crc16(b,n);append((uint8_t)(c>>8));append((uint8_t)(c^(crc_bad?1:0)));}
static void cs(void *c,bool on){(void)c;selected=on;if(!on){used=pos=0;writing=crcbytes=0;}}
static uint64_t us(void *c){(void)c;return clock_us;}
static int exchange(void *c,const uint8_t *tx,uint8_t *rx,uint16_t n)
{
 (void)c;clock_us+=(uint64_t)n*2;
 if(io_fail)return -1;
 memset(rx,255,n);if(!selected)return 0;
 if(n==6&&(tx[0]&0xc0)==0x40){
  assert(pos==used);used=pos=0;assert(sd_crc7(tx,5)==tx[5]);
  uint32_t arg=(uint32_t)tx[1]<<24|(uint32_t)tx[2]<<16|(uint32_t)tx[3]<<8|tx[4];
  unsigned cmd=tx[0]&63;if(no_response)return 0;
  if(cmd==0){initialized=false;append(1);}
  else if(cmd==8){append(1);append(0);append(0);append(1);append(0xaa);}
  else if(cmd==55)append(initialized?0:1);
  else if(cmd==41){assert(arg==0x40000000);initialized=true;append(0);}
  else if(cmd==58){append(0);append(0xc0);append(0xff);append(0x80);append(0);}
  else if(cmd==59){assert(arg==1);append(0);}
  else if(cmd==10){uint8_t cid[16]={3,0x53,0x44,0x53,0x44,0x33,0x32,0x47,0x85,0x8b,0xf0,0x92,0x3c,1,0x92,0x7b};append(0);block(cid,16);}
  else if(cmd==9){uint8_t csd[16]={0x40};uint32_t size=62333952u/1024u-1;csd[7]=(size>>16)&63;csd[8]=(uint8_t)(size>>8);csd[9]=(uint8_t)size;append(0);block(csd,16);}
  else if(cmd==17){assert(arg==0||(arg>=8192&&arg<8320));append(0);if(!no_token)block(memory[arg?arg-8191:0],512);}
  else if(cmd==24){pending_lba=arg;writing=1;write_offset=0;append(0);}
  else if(cmd==13){append(0);append(0);}
  else assert(!"Unexpected command");
  return 0;
 }
 if(writing&&pos==used){
  if(writing==1){assert(n==1&&tx[0]==0xfe);writing=2;}
  else if(writing==2){assert(n==128);memcpy(memory[pending_lba-8191]+write_offset,tx,n);write_offset+=n;if(write_offset==512)writing=3;}
  else if(writing==3){assert(n==2);actual_write_crc=(uint16_t)((uint16_t)tx[0]<<8|tx[1]);assert(actual_write_crc==sd_crc16(memory[pending_lba-8191],512));writing=0;used=pos=0;append(5);append(0);busy_until=clock_us+inject_busy_us;inject_busy_us=0;}

  return 0;
 }
 for(unsigned i=0;i<n;i++){if(pos<used)rx[i]=q[pos++];else if(busy_forever||clock_us<busy_until)rx[i]=0;}
 return 0;
}
static SdSpi s;
static void start(void){used=pos=writing=crcbytes=0;selected=initialized=crc_bad=no_token=no_response=busy_forever=io_fail=stuck_after_data=false;inject_busy_us=busy_until=0;clock_us=0x100000000ull;assert(!sd_spi_init(&s,(SdSpiIo){NULL,exchange,cs,us}));assert(!selected&&s.sectors==62333952u&&!s.write_enabled);}
#define RAW_TARGET 53328u
#define PAYLOAD_TARGET 31998u
static CaptureQueue cq;
static uint8_t source[32004];
static uint32_t produced,model_backlog,source_count;
static FirDecimator10 dsp;static FifoEventTime model_time;static FifoLive owner;
static Tr2Result model_sink(void *unused,uint64_t index,const int16_t xyz[3],uint64_t time)
{
 (void)unused;assert(time==5000000u+index*75u/2u);
 FirDecimator10Input in={.source_index=index,.time_us=time,.xyz={xyz[0],xyz[1],xyz[2]},.valid=true,.time_valid=true};
 FirDecimator10Output out;bool ready;assert(fir_decimator10_push(&dsp,&in,&out,&ready)==TR2_OK);
 if(ready){assert(out.source_index==index&&out.causal_time_us==time);assert(R3_QUEUE_CAPACITY-model_backlog>=6);
  for(unsigned axis=0;axis<3;axis++)for(unsigned k=0;k<2;k++){
   uint8_t b=(uint8_t)((uint16_t)out.xyz[axis]>>(8*k));assert(produced<sizeof source);assert(capture_queue_push(&cq,&b,1));source[produced++]=b;model_backlog=cq.count;
  }
 }return TR2_OK;
}
static void model_word(bool anchor)
{
 uint8_t b[7]={0};b[0]=anchor?32:(uint8_t)(16+2*(source_count%4));unsigned parity=b[0];parity^=parity>>4;parity^=parity>>2;parity^=parity>>1;b[0]|=parity&1;
 if(anchor){uint32_t raw=100+source_count*3;for(unsigned k=0;k<4;k++)b[k+1]=(uint8_t)(raw>>(8*k));b[6]=10;}
 else {int16_t xyz[3]={(int16_t)(1000+source_count%97),-500,23};for(unsigned j=0;j<3;j++){b[1+2*j]=(uint8_t)xyz[j];b[2+2*j]=(uint8_t)((uint16_t)xyz[j]>>8);}}
 assert(!fifo_window_parse_word(b,&owner.parser));assert(!fifo_event_time_consume(&model_time,b,1,source_count));
}
static void scenario(uint32_t injected)
{
 start();assert(!sd_writer_r3_gate(&s));SdWriter w={0};inject_busy_us=injected;
 capture_queue_init(&cq);produced=model_backlog=source_count=0;uint32_t consumed=0,maxback=0,polls=0,maxgap=0;bool final_anchor=false;
 memset(&owner,0,sizeof owner);fir_decimator10_init(&dsp);assert(fir_decimator10_start(&dsp,1,0,(FirDecimator10Clock){80000,3,false})==TR2_OK);
 assert(fifo_event_time_init_sink(&model_time,&owner,model_sink,NULL,100,5000000,1000000,80000)==TR2_OK);uint64_t begin=clock_us,previous=begin;
 while(source_count<RAW_TARGET||consumed<produced){
  /* Separate foreground FIFO service keeps receiving while writer is pending.
   * Synthetic time/data only; no claim of physical DWT or sensor accuracy. */
  uint32_t gap=(uint32_t)(clock_us-previous);if(gap>maxgap)maxgap=gap;previous=clock_us;polls++;clock_us+=300;
  uint64_t elapsed=clock_us-begin;uint32_t due=elapsed>=2000000?RAW_TARGET:(uint32_t)(elapsed*80000u/3000000u);if(due>RAW_TARGET)due=RAW_TARGET;
  while(source_count<due){if(source_count%8==0)model_word(true);model_word(false);source_count++;}
  if(source_count==RAW_TARGET&&!final_anchor){model_word(true);final_anchor=true;}
  if(model_backlog>maxback)maxback=model_backlog;
  if(!w.active&&consumed<produced&&(model_backlog>=512||produced==PAYLOAD_TARGET)){
   static uint8_t sector[512];unsigned n=model_backlog<512?model_backlog:512;
   (void)n;const uint8_t *borrowed=capture_queue_acquire(&cq,produced==PAYLOAD_TARGET,sector);assert(borrowed);
   assert(!sd_writer_begin(&w,&s,8192+consumed/512,borrowed));
  }
  if(w.active){assert(!sd_writer_step(&w));if(w.done){unsigned n=cq.borrowed;assert(capture_queue_commit(&cq));consumed+=n;model_backlog=cq.count;}}
  clock_us+=100;
 }
 assert(!selected&&w.done&&!s.failed&&polls>1000&&maxgap<5000);
 assert(!memcmp(source,memory[1],512));
 assert(produced==PAYLOAD_TARGET);
 for(unsigned i=0;i<PAYLOAD_TARGET;i++)assert(source[i]==memory[1+i/512][i%512]);
 R3Hash h;r3_hash_init(&h);for(unsigned i=0;i<PAYLOAD_TARGET;i+=7)r3_hash_update(&h,source+i,PAYLOAD_TARGET-i<7?PAYLOAD_TARGET-i:7);
 uint8_t a[32],b[32];r3_hash_finish(&h,a);assert(tr2_sha256_bytes(source,PAYLOAD_TARGET,b)==TR2_OK&&!memcmp(a,b,32));
 uint8_t bad[7]={0};uint64_t before=model_time.emitted;
 assert(fifo_event_time_consume(&model_time,bad,1,source_count)==-1&&model_time.fault&&model_time.emitted==before);
 fir_decimator10_invalidate(&dsp,TR2_D10_SOURCE_FAULT);assert(!dsp.active);
 printf("Injected busy%u us: polls%u maxgap%u us peakqueue%u exact31998bytes from FIFO DEC8 + real Q15, no retry (MODEL ONLY)\n",injected,polls,maxgap,maxback);
}
int main(void)
{
 scenario(150000);scenario(500000);
 start();SdWriter w={0};uint8_t b[512]={0};assert(sd_writer_begin(&w,&s,8192,b)==SD_REFUSED);
 s.cid[0]^=1;assert(sd_writer_r3_gate(&s)==SD_IDENTITY&&!s.write_enabled);s.cid[0]^=1;assert(!sd_writer_r3_gate(&s));
 assert(sd_writer_begin(&w,&s,2048,b)==SD_RANGE);assert(!sd_writer_begin(&w,&s,8192,b));
 io_fail=true;assert(sd_writer_step(&w)==SD_IO&&!w.active&&!selected&&s.failed);
 assert(sd_writer_begin(&w,&s,8192,b)==SD_REFUSED);
 start();memset(&w,0,sizeof w);assert(!sd_writer_r3_gate(&s));assert(!sd_writer_begin(&w,&s,8192,b));
 busy_forever=true;for(unsigned i=0;i<2000&&w.active;i++){clock_us+=1000;(void)sd_writer_step(&w);}
 assert(w.error==SD_TIMEOUT&&!w.active&&!selected&&s.failed);
 /* A full queue explicitly refuses before overwriting its oldest bytes. */
 capture_queue_init(&cq);uint8_t sector[512]={0};for(unsigned i=0;i<64;i++)assert(capture_queue_push(&cq,sector,512));
 assert(cq.count==32768&&capture_queue_acquire(&cq,false,sector));
 assert(!capture_queue_push(&cq,sector,1)&&cq.overflows==1&&cq.count==32768);
 assert(capture_queue_commit(&cq)&&cq.count==32256);assert(capture_queue_push(&cq,sector,512)&&cq.count==32768);
 R3Hash h;r3_hash_init(&h);uint8_t digest[32];r3_hash_update(&h,(const uint8_t *)"abc",3);r3_hash_finish(&h,digest);
 static const uint8_t abc[32]={0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad};assert(!memcmp(digest,abc,32));
 puts("R3 cooperative SD quanta/CID/region/fault latch and streaming SHA known vector PASS; simulation only");return 0;
}
