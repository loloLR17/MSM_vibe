#include "sd_writer.h"
#include "sd_group.h"
#include "stream_hash.h"
#include "capture_queue.h"
#include "fat_volume.h"
#include "archive_format.h"
#include "ff.h"
#include "endurance_clock.h"
#include <stdlib.h>
#include "tr2/common/sha256.h"
#include "fifo_event_time.h"
#include "tr2/domain/acquisition/fir_decimator10.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static FILE *image;static uint8_t sector[512];
static uint8_t q[2048];static unsigned used,pos,writing,crcbytes;
static bool multi;static uint32_t preerase,repeat_left,repeat_us;
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
  else if(cmd==17){assert(!fseek(image,(long)arg*512,SEEK_SET));assert(fread(sector,1,512,image)==512);append(0);if(!no_token)block(sector,512);}
  else if(cmd==23){preerase=arg;append(0);}
  else if(cmd==25){assert(preerase>=1&&preerase<=8);pending_lba=arg;multi=true;append(0);}
  else if(cmd==24){pending_lba=arg;writing=1;write_offset=0;append(0);}
  else if(cmd==13){append(0);append(0);}
  else assert(!"Unexpected command");
  return 0;
 }
 if(multi&&pos==used&&n==1&&tx[0]==0xfc){writing=2;write_offset=0;return 0;}
 if(multi&&pos==used&&n==1&&tx[0]==0xfd){multi=false;return 0;}
 if(writing&&pos==used){
  if(writing==1){assert(n==1&&tx[0]==0xfe);writing=2;}
  else if(writing==2){assert(n==128);memcpy(sector+write_offset,tx,n);write_offset+=n;if(write_offset==512)writing=3;}
  else if(writing==3){assert(n==2);actual_write_crc=(uint16_t)((uint16_t)tx[0]<<8|tx[1]);assert(actual_write_crc==sd_crc16(sector,512));assert(!fseek(image,(long)pending_lba*512,SEEK_SET));assert(fwrite(sector,1,512,image)==512);if(multi)pending_lba++;writing=0;used=pos=0;append(5);append(0);busy_until=clock_us+(inject_busy_us?inject_busy_us:(repeat_left?(repeat_left--,repeat_us):0));inject_busy_us=0;}

  return 0;
 }
 for(unsigned i=0;i<n;i++){if(pos<used)rx[i]=q[pos++];else if(busy_forever||clock_us<busy_until)rx[i]=0;}
 return 0;
}
static SdSpi s;
static void start(void){used=pos=writing=crcbytes=0;selected=initialized=crc_bad=no_token=no_response=busy_forever=io_fail=stuck_after_data=false;multi=false;preerase=repeat_left=repeat_us=0;inject_busy_us=busy_until=0;clock_us=0x100000000ull;assert(!sd_spi_init(&s,(SdSpiIo){NULL,exchange,cs,us}));assert(!selected&&s.sectors==62333952u&&!s.write_enabled);}

static void make_header(uint8_t h[512])
{
 memset(h,0,512);memcpy(h,"TR2DUR01",8);durable_put32(h+8,1);durable_put32(h+12,512);durable_put32(h+16,3);durable_put32(h+20,2000000);
 memcpy(h+36,s.cid,16);memset(h+52,'a',40);durable_put32(h+124,8);durable_put32(h+128,244);durable_put32(h+132,10);durable_put64(h+136,80000);durable_put64(h+144,3);durable_put64(h+152,123456);durable_put32(h+160,78);durable_put32(h+508,durable_crc(h,508));
}
static void writer_stop_states(void)
{
 for(unsigned state=1;state<=6;state++){
  start();assert(!sd_writer_r3_gate(&s));s.begin_lba=0;s.limit_lba=s.sectors;SdWriter w={0};uint8_t data[512]={0};unsigned before=s.writes;
  assert(!sd_writer_begin(&w,&s,60000,data));while(w.state!=state){assert(!sd_writer_step(&w));clock_us+=500;}
  /* STOP source: finish only this already borrowed sector, never a new write. */
  while(w.active){assert(!sd_writer_step(&w));clock_us+=500;}
  assert(w.done&&!w.uncertain&&!selected&&s.writes==before+1);
 }
 start();assert(!sd_writer_r3_gate(&s));s.begin_lba=0;s.limit_lba=s.sectors;SdWriter w={0};uint8_t data[512]={0};assert(!sd_writer_begin(&w,&s,60000,data));while(w.state!=4)assert(!sd_writer_step(&w));io_fail=true;assert(sd_writer_step(&w)==SD_IO&&w.uncertain&&s.failed&&!w.active);assert(sd_writer_begin(&w,&s,60000,data)==SD_REFUSED);
 puts("STOP all6writer states +uncertainCRC latch/no retry PASS (MODEL)");
}
static void framed_busy(uint32_t hold,unsigned holds,bool expect_overflow)
{
 start();assert(!sd_writer_r3_gate(&s));s.begin_lba=0;s.limit_lba=s.sectors;CaptureQueue queue;capture_queue_init(&queue);SdWriter w={0};uint8_t data[512]={0},scratch[512];unsigned produced=0,count=0,seq=0,done=0;uint64_t begin=clock_us;bool overflow=false;unsigned writes=0;
 while(produced<20000||count||queue.count||w.active){
  clock_us+=400;uint64_t due=(clock_us-begin)*8000/3000000;if(due>20000)due=20000;
  while(produced<due){for(unsigned a=0;a<3;a++){int16_t value=(int16_t)(produced+a*100);data[44+count*6+2*a]=(uint8_t)value;data[45+count*6+2*a]=(uint8_t)((uint16_t)value>>8);}produced++;count++;
   if(count==78){durable_record(data,99,seq,produced-count,5000000+(uint64_t)(produced-count)*375,5000000+(uint64_t)(produced-1)*375,count,seq?0:1);if(!capture_queue_push(&queue,data,512)){overflow=true;break;}count=0;seq++;}}
  if(overflow)break;
  if(produced==20000&&count){durable_record(data,99,seq,produced-count,5000000+(uint64_t)(produced-count)*375,5000000+(uint64_t)(produced-1)*375,count,0);if(!capture_queue_push(&queue,data,512)){overflow=true;break;}count=0;seq++;}
  if(!w.active&&queue.count){const uint8_t *borrowed=capture_queue_acquire(&queue,false,scratch);assert(borrowed);inject_busy_us=writes++<holds?hold:0;assert(!sd_writer_begin(&w,&s,60000+done,borrowed));}
  if(w.active){assert(!sd_writer_step(&w));if(w.done){assert(capture_queue_commit(&queue));done++;}}
  assert(clock_us-begin<30000000);
 }
 assert(overflow==expect_overflow);
 if(!overflow){assert(produced==20000&&done==seq&&!queue.count);unsigned total=0;for(unsigned i=0;i<done;i++){assert(!sd_spi_read(&s,60000+i,data));assert(durable_record_valid(data,99,i,total));unsigned n=data[36];for(unsigned j=0;j<n;j++)for(unsigned a=0;a<3;a++){int16_t actual=(int16_t)((uint16_t)data[44+j*6+2*a]|(uint16_t)data[45+j*6+2*a]<<8);assert(actual==(int16_t)(total+j+a*100));}total+=n;}assert(total==20000);}
 else {assert(queue.overflows==1);while(w.active){assert(!sd_writer_step(&w));clock_us+=1000;}}
 printf("Framed 78XYZ MODEL hold%u repeats%u peak%u overflow%u exact_outputs%u\n",hold,holds,queue.peak,overflow,overflow?0:produced);
}
static void framed_group_busy(uint32_t hold,unsigned holds,bool expect_overflow)
{
 start();assert(!sd_writer_r3_gate(&s));s.begin_lba=0;s.limit_lba=s.sectors;CaptureQueue queue;capture_queue_init(&queue);SdGroup w={0};uint8_t data[512]={0};unsigned produced=0,count=0,seq=0,done=0;uint64_t begin=clock_us;bool overflow=false;repeat_left=holds;repeat_us=hold;
 while(produced<20000||count||queue.count||w.active){
  clock_us+=400;uint64_t due=(clock_us-begin)*8000/3000000;if(due>20000)due=20000;
  while(produced<due){for(unsigned a=0;a<3;a++){int16_t value=(int16_t)(produced+a*100);data[44+count*6+2*a]=(uint8_t)value;data[45+count*6+2*a]=(uint8_t)((uint16_t)value>>8);}produced++;count++;
   if(count==78){durable_record(data,99,seq,produced-count,5000000+(uint64_t)(produced-count)*375,5000000+(uint64_t)(produced-1)*375,count,seq?0:1);if(!capture_queue_push(&queue,data,512)){overflow=true;break;}count=0;seq++;}}
  if(overflow)break;
  if(produced==20000&&count){durable_record(data,99,seq,produced-count,5000000+(uint64_t)(produced-count)*375,5000000+(uint64_t)(produced-1)*375,count,0);if(!capture_queue_push(&queue,data,512)){overflow=true;break;}count=0;seq++;}
  if(!w.active&&(queue.count>=4096||(produced==20000&&queue.count))){unsigned blocks=queue.count/512;if(blocks>8)blocks=8;unsigned contiguous=(R3_QUEUE_CAPACITY-queue.tail)/512;if(blocks>contiguous)blocks=contiguous;assert(!queue.borrowed);queue.borrowed=blocks*512;assert(!sd_group_begin(&w,&s,60000+done,queue.data+queue.tail,blocks));}
  if(w.active){assert(!sd_group_step(&w));if(w.done){assert(capture_queue_commit(&queue));done+=w.blocks;}}
  assert(clock_us-begin<30000000);
 }
 assert(overflow==expect_overflow);
 if(!overflow){assert(produced==20000&&done==seq&&!queue.count);unsigned total=0;for(unsigned i=0;i<done;i++){assert(!sd_spi_read(&s,60000+i,data));assert(durable_record_valid(data,99,i,total));unsigned n=data[36];for(unsigned j=0;j<n;j++)for(unsigned a=0;a<3;a++){int16_t actual=(int16_t)((uint16_t)data[44+j*6+2*a]|(uint16_t)data[45+j*6+2*a]<<8);assert(actual==(int16_t)(total+j+a*100));}total+=n;}assert(total==20000);}
 else {assert(queue.overflows==1);while(w.active){assert(!sd_group_step(&w));clock_us+=1000;}}
 printf("Framed grouped78XYZ MODEL hold%u repeats%u peak%u overflow%u exact_outputs%u\n",hold,holds,queue.peak,overflow,overflow?0:produced);
}
static void group_cases(void)
{
 static uint8_t data[4096],read[512];for(unsigned i=0;i<sizeof data;i++)data[i]=(uint8_t)(i*19+i/137);
 for(unsigned blocks=1;blocks<=8;blocks++)for(unsigned stop_state=1;stop_state<=10;stop_state++){
  start();assert(!sd_writer_r3_gate(&s));s.begin_lba=0;s.limit_lba=s.sectors;SdGroup w={0};inject_busy_us=150000;
  assert(!sd_group_begin(&w,&s,61000,data,blocks));while(w.state!=stop_state){assert(!sd_group_step(&w));clock_us+=400;}
  /* Stop source only; finish the already borrowed8sectors and explicit FD. */
  while(w.active){assert(!sd_group_step(&w));clock_us+=400;}
  assert(w.done&&!w.uncertain&&!multi&&!selected&&s.writes==blocks&&w.block==blocks);
  for(unsigned j=0;j<blocks;j++){assert(!sd_spi_read(&s,61000+j,read));assert(!memcmp(read,data+512*j,512));}
 }
 start();assert(!sd_writer_r3_gate(&s));s.begin_lba=0;s.limit_lba=s.sectors;SdGroup w={0};assert(!sd_group_begin(&w,&s,61000,data,8));while(w.state!=5)assert(!sd_group_step(&w));io_fail=true;assert(sd_group_step(&w)==SD_IO&&w.needs_power_cycle&&w.uncertain&&!w.active&&s.failed);assert(sd_group_begin(&w,&s,61000,data,8)==SD_REFUSED);
 puts("Cooperative ACMD23/CMD25/FC/FD/CMD13 8sizes/all10STOPstates exact +uncertaindataIO no retry PASS (MODEL)");
}
int main(int argc,char **argv)
{
 /* Exact actual clock accumulator across many DWT wraps, including boot
  * offset beyond absolute u32 micros and one-hour duration near its bound. */
 uint64_t elapsed=0;uint32_t previous=0xfffffff0u;
 for(uint64_t i=1;i<=5400000;i++){uint32_t current=previous+160000u;elapsed=durable_elapsed_cycles(elapsed,current,previous);previous=current;assert(elapsed==i*160000u);}
 assert(elapsed/160u==5400000000ull&&3600000000ull<UINT32_MAX);
 assert(((9600000ull+77)/78)*512+1024<72ull*1024*1024);
 assert(argc==3);image=fopen(argv[1],"w+b");assert(image);assert(!fseek(image,62333952L*512-1,SEEK_SET));assert(fputc(0,image)==0);assert(!fflush(image));start();assert(durable_mount(&s)==1);assert(!durable_format());assert(durable_status.mounted&&durable_status.fs_type==FS_FAT32);
 uint8_t header[512];make_header(header);assert(!durable_prepare(3,72u*1024u*1024u,123456,header));uint32_t base=durable_status.first_lba;
 uint8_t data[512]={0},packed[6];R3Hash hash;r3_hash_init(&hash);
 for(unsigned i=0;i<78;i++){int16_t xyz[3]={(int16_t)(1000+i),-500,23};for(unsigned a=0;a<3;a++){packed[2*a]=(uint8_t)xyz[a];packed[2*a+1]=(uint8_t)((uint16_t)xyz[a]>>8);}memcpy(data+44+i*6,packed,6);r3_hash_update(&hash,packed,6);}
 durable_record(data,123456,0,0,5000000,5028875,78,1);assert(durable_record_valid(data,123456,0,0));assert(!durable_record_valid(data,123457,0,0));assert(!durable_record_valid(data,123456,1,0));data[55]^=1;assert(!durable_record_valid(data,123456,0,0));data[55]^=1;
 assert(!durable_sector(base+1,data));uint8_t footer[512]={0};memcpy(footer,"TR2END01",8);durable_put32(footer+8,1);durable_put32(footer+12,1);durable_put64(footer+16,123456);durable_put32(footer+24,3);durable_put32(footer+28,78);durable_put32(footer+32,780);durable_put32(footer+36,1);durable_put32(footer+40,468);durable_put32(footer+44,2000000);durable_put32(footer+48,1);durable_put64(footer+56,5000000);durable_put64(footer+64,5028875);r3_hash_finish(&hash,footer+96);durable_put32(footer+508,durable_crc(footer,508));assert(!durable_sector(base+72u*1024u*1024u/512u-1,footer));assert(!durable_verify_last(footer+96));
 FATFS check;FIL f;assert(f_mount(&check,"0:",1)==FR_OK);assert(f_open(&f,"0:/RUN0003.TR2",FA_READ)==FR_OK&&f_size(&f)==72u*1024u*1024u);FILE *out=fopen(argv[2],"wb");assert(out);UINT n;
 do{assert(f_read(&f,data,512,&n)==FR_OK);assert(fwrite(data,1,n,out)==n);}while(n);assert(!fclose(out));assert(f_close(&f)==FR_OK);group_cases();framed_group_busy(150000,1,false);framed_group_busy(500000,1,false);framed_group_busy(151054,19,false);framed_group_busy(500000,19,true);writer_stop_states();framed_busy(150000,1,false);framed_busy(500000,1,false);framed_busy(151054,19,false);framed_busy(500000,19,true);
 /* Real640MiB FAT32 contiguous chain; metadata persisted before capture.
  * This is allocation only, no synthetic8h declared a physical run. */
 assert(!durable_mount(&s));assert(!durable_prepare(4,640u*1024u*1024u,123457,header));
 assert(durable_status.required_clusters==20480&&durable_status.sectors==1310720);
 assert(f_open(&f,"0:/RUN0004.TR2",FA_READ)==FR_OK&&f_size(&f)==640u*1024u*1024u);
 assert(f_lseek(&f,f_size(&f)-512)==FR_OK&&f_read(&f,data,512,&n)==FR_OK&&n==512);
 for(unsigned i=0;i<512;i++)assert(data[i]==0);
 assert(f_close(&f)==FR_OK);
 puts("Real640MiB FAT32 preallocation/all20480contiguous clusters/invalid footer PASS (MODEL)");
 /* Existing name is refused, preserving its original file and all media bytes. */
 assert(!durable_mount(&s));unsigned writes=s.writes;assert(durable_prepare(3,131072,123456,header)==-1&&durable_status.error==8&&s.writes==writes);
 /* Allocate actual model FAT chains until full, without fabricating a result. */
 for(unsigned i=0;i<7;i++){char name[]="0:/FULL0000.BIN";name[10]=(char)('0'+i);assert(f_open(&f,name,FA_WRITE|FA_CREATE_NEW)==FR_OK);assert(f_expand(&f,0xffff0000u,1)==FR_OK);assert(f_close(&f)==FR_OK);}
 DWORD free_clusters;FATFS *full;assert(f_getfree("0:",&free_clusters,&full)==FR_OK);assert(free_clusters&&free_clusters<(0xffffffffu/(full->csize*512u)));
 assert(f_open(&f,"0:/LAST.BIN",FA_WRITE|FA_CREATE_NEW)==FR_OK);assert(f_expand(&f,free_clusters*full->csize*512u,1)==FR_OK);assert(f_close(&f)==FR_OK);
 writes=s.writes;assert(durable_prepare(4,2097152,123457,header)==-1&&durable_status.error==30);assert(s.writes==writes);writes=s.writes;
 s.cid[0]^=1;assert(durable_mount(&s)==-1&&durable_status.error==1&&!s.write_enabled&&s.writes==writes);
 puts("Real FAT allocation full / duplicate name / wrong CID refuse before capture PASS (MODEL)");assert(!fclose(image));
 puts("ST FatFs format/mount/contiguous expand/sync/close/remount/standard-file-read exact fixture PASS (HOST MODEL ONLY)");return 0;
}
