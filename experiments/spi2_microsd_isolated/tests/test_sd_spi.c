#include "sd_spi.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint8_t q[2048],memory[16][512];static unsigned used,pos,writing,crcbytes;
static bool selected,initialized,crc_bad,no_token,no_response,busy_forever,io_fail,stuck_after_data;
static uint64_t clock_us;static uint32_t pending_lba;static uint16_t actual_write_crc;
static void append(uint8_t b){assert(used<sizeof q);q[used++]=b;}
static void block(const uint8_t *b,unsigned n){append(0xfe);for(unsigned i=0;i<n;i++)append(b[i]);uint16_t c=sd_crc16(b,n);append((uint8_t)(c>>8));append((uint8_t)(c^(crc_bad?1:0)));}
static void cs(void *c,bool on){(void)c;selected=on;if(!on){used=pos=0;writing=crcbytes=0;}}
static uint64_t us(void *c){(void)c;return clock_us;}
static int exchange(void *c,const uint8_t *tx,uint8_t *rx,uint16_t n)
{
 (void)c;clock_us+=(uint64_t)n*8;
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
  else if(cmd==17){assert(arg==0||(arg>=2048&&arg<2063));append(0);if(!no_token)block(memory[arg?arg-2047:0],512);}
  else if(cmd==24){pending_lba=arg;writing=1;append(0);}
  else if(cmd==13){append(0);append(0);}
  else assert(!"Unexpected command");
  return 0;
 }
 if(writing&&pos==used){
  if(writing==1){assert(n==1&&tx[0]==0xfe);writing=2;}
  else if(writing==2){assert(n==512);memcpy(memory[pending_lba?pending_lba-2047:0],tx,n);writing=3;}
  else if(writing==3){assert(n==1);actual_write_crc=(uint16_t)((actual_write_crc<<8)|tx[0]);crcbytes++;if(crcbytes==2){assert(actual_write_crc==sd_crc16(memory[pending_lba?pending_lba-2047:0],512));actual_write_crc=0;if(stuck_after_data)busy_forever=true;writing=0;used=pos=0;append(5);append(0);append(0);if(!busy_forever)append(255);}}
  return 0;
 }
 for(unsigned i=0;i<n;i++){if(pos<used)rx[i]=q[pos++];else if(busy_forever)rx[i]=0;}
 return 0;
}
static SdSpi s;
static void start(void){used=pos=writing=crcbytes=0;selected=initialized=crc_bad=no_token=no_response=busy_forever=io_fail=stuck_after_data=false;clock_us=0x100000000ull;assert(!sd_spi_init(&s,(SdSpiIo){NULL,exchange,cs,us}));assert(!selected&&s.sectors==62333952u&&!s.write_enabled);}
int main(void)
{
 const uint8_t cmd0[5]={0x40,0,0,0,0},cmd8[5]={0x48,0,0,1,0xaa};assert(sd_crc7(cmd0,5)==0x95&&sd_crc7(cmd8,5)==0x87);
 assert(sd_crc16((const uint8_t *)"123456789",9)==0x31c3);
 start();uint8_t b[512],back[512];for(unsigned i=0;i<512;i++)b[i]=(uint8_t)(i*37u+5u);
 assert(sd_spi_write(&s,2048,b)==SD_REFUSED);
 assert(!sd_spi_test_gate(&s,2048,15));assert(sd_spi_write(&s,1,b)==SD_RANGE);
 assert(!sd_spi_write(&s,2048,b));assert(!sd_spi_read(&s,2048,back));assert(!memcmp(b,back,512)&&s.last_busy_us>0&&s.writes==1);
 s.cid[0]^=1;assert(sd_spi_test_gate(&s,2048,15)==SD_IDENTITY&&!s.write_enabled);
 start();crc_bad=true;assert(sd_spi_read(&s,2048,back)==SD_CRC&&s.failed&&!selected&&s.crc_errors==1);assert(sd_spi_read(&s,2048,back)==SD_REFUSED);
 start();no_token=true;assert(sd_spi_read(&s,2048,back)==SD_TIMEOUT&&s.failed&&!selected);
 start();no_response=true;assert(sd_spi_read(&s,2048,back)==SD_TIMEOUT&&s.failed&&!selected);
 start();io_fail=true;assert(sd_spi_read(&s,2048,back)==SD_IO&&!selected);
 start();assert(!sd_spi_test_gate(&s,2048,15));busy_forever=true;uint64_t began=clock_us;
 /* Busy fault before a command is itself bounded, without a write. */
 assert(sd_spi_write(&s,2048,b)!=SD_OK&&s.failed&&!selected&&clock_us-began<510000u);
 start();assert(!sd_spi_test_gate(&s,2048,15));stuck_after_data=true;began=clock_us;
 assert(sd_spi_write(&s,2048,b)==SD_TIMEOUT&&s.failed&&!selected&&s.writes==0&&clock_us-began<510000u);
 busy_forever=false;assert(!sd_spi_quiesce(&s)&&s.failed&&s.error==SD_TIMEOUT&&!selected);
 assert(sd_spi_write(&s,2048,b)==SD_REFUSED);
 puts("SDHC SPI model: protocol CRC7/16, read/write reversal, CID gate, region bounds, fail-closed CRC/token/response/I/O/busy; NOT hardware proof");
 return 0;
}
