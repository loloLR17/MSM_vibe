/* R2 isolated SDHC SPI transport. Protocol follows the ST Adafruit802 SPI SD
 * reference, with explicit deadlines/CRC/SDHC-only/identity guards. Not HAL_SD.
 * No copied BSP implementation and no implicit retry of a failed data write. */
#include "sd_spi.h"
#include <string.h>
uint8_t sd_crc7(const uint8_t *b,size_t n)
{
 uint8_t crc=0;
 for(size_t i=0;i<n;i++){uint8_t d=b[i];for(unsigned k=0;k<8;k++){
  crc<<=1;if((d^crc)&0x80u)crc^=9u;d<<=1;
 }}
 return (uint8_t)((crc<<1)|1u);
}
uint16_t sd_crc16(const uint8_t *b,size_t n)
{
 uint16_t crc=0;for(size_t i=0;i<n;i++){crc^=(uint16_t)b[i]<<8;
  for(unsigned k=0;k<8;k++)crc=(uint16_t)((crc<<1)^((crc&0x8000u)?0x1021u:0));
 }return crc;
}
static int fail(SdSpi *s,int e){if(!s->failed){s->error=(uint32_t)e;s->failed=true;if(e==SD_TIMEOUT)s->timeouts++;if(e==SD_CRC)s->crc_errors++;}return (int)s->error;}
static int byte(SdSpi *s,uint8_t tx,uint8_t *rx){return s->io.exchange(s->io.context,&tx,rx,1)?fail(s,SD_IO):0;}
static int close_card(SdSpi *s,int result)
{
 s->io.select(s->io.context,false);uint8_t discard;
 int r=byte(s,255,&discard);return result?result:r;
}
static int ready(SdSpi *s,uint32_t deadline_us,bool busy)
{
 uint64_t start=s->io.us(s->io.context);uint8_t v=0;
 for(uint32_t n=0;n<1000000u;n++){
  if(byte(s,255,&v))return SD_IO;
  uint64_t elapsed=s->io.us(s->io.context)-start;
  if(v==255){if(busy){s->last_busy_us=(uint32_t)elapsed;if(elapsed>s->max_busy_us)s->max_busy_us=(uint32_t)elapsed;}return 0;}
  if(elapsed>=deadline_us)return fail(s,SD_TIMEOUT);
 }
 return fail(s,SD_TIMEOUT); /* secondary bound if an injected clock is stuck */
}
static int command(SdSpi *s,uint8_t cmd,uint32_t arg,uint8_t extra_bytes,uint32_t *extra)
{
 uint64_t start=s->io.us(s->io.context);s->last_cmd=cmd;s->last_r1=255;
 s->io.select(s->io.context,true);
 int readiness=ready(s,500000,false);if(readiness)return readiness;
 uint8_t frame[6]={0x40u|cmd,(uint8_t)(arg>>24),(uint8_t)(arg>>16),(uint8_t)(arg>>8),(uint8_t)arg,0},rx[6];
 frame[5]=sd_crc7(frame,5);
 if(s->io.exchange(s->io.context,frame,rx,6))return fail(s,SD_IO);
 uint8_t r=255;for(unsigned i=0;i<8;i++){if(byte(s,255,&r))return SD_IO;if(!(r&0x80u))break;}
 uint32_t value=0;
 if(r&0x80u)return fail(s,SD_TIMEOUT);
 for(unsigned i=0;i<extra_bytes;i++){uint8_t v;if(byte(s,255,&v))return SD_IO;value=(value<<8)|v;}
 s->last_r1=r;if(extra)*extra=value;
 uint32_t slot=s->commands<64?s->commands:64+(s->commands-64)%192;
 s->trace[slot]=(SdTrace){cmd,arg,r,value,(uint32_t)(s->io.us(s->io.context)-start)};s->commands++;
 return 0;
}
static int simple(SdSpi *s,uint8_t cmd,uint32_t arg,uint8_t r1,uint8_t extra_bytes,uint32_t *extra)
{
 int r=command(s,cmd,arg,extra_bytes,extra);if(!r&&s->last_r1!=r1)r=fail(s,SD_RESPONSE);return close_card(s,r);
}
static int data(SdSpi *s,uint8_t *out,uint16_t size)
{
 uint64_t start=s->io.us(s->io.context);uint8_t token=255;
 for(uint32_t n=0;n<1000000u;n++){
  if(byte(s,255,&token))return SD_IO;
  if(token==0xfe)break;
  if(token!=255)return fail(s,SD_TOKEN);
  if(s->io.us(s->io.context)-start>=200000u)return fail(s,SD_TIMEOUT);
 }
 if(token!=0xfe)return fail(s,SD_TIMEOUT);
 uint8_t ff[512];memset(ff,255,sizeof ff);
 if(!size||size>512)return fail(s,SD_RANGE);
 if(s->io.exchange(s->io.context,ff,out,size))return fail(s,SD_IO);
 uint8_t hi,lo;if(byte(s,255,&hi)||byte(s,255,&lo))return SD_IO;
 if(sd_crc16(out,size)!=((uint16_t)hi<<8|lo))return fail(s,SD_CRC);
 return 0;
}
static int reg_data(SdSpi *s,uint8_t cmd,uint8_t out[16])
{int r=command(s,cmd,0,0,NULL);if(!r&&s->last_r1)r=fail(s,SD_RESPONSE);if(!r)r=data(s,out,16);return close_card(s,r);}
int sd_spi_init(SdSpi *s,SdSpiIo io)
{
 if(!s||!io.exchange||!io.select||!io.us)return SD_REFUSED;
 memset(s,0,sizeof *s);s->io=io;io.select(io.context,false);
 uint8_t ff[10],rx[10];memset(ff,255,sizeof ff);
 if(io.exchange(io.context,ff,rx,10))return fail(s,SD_IO);
 /* Whole session is new only after a deliberate power-defined SPI transition. */
 if(simple(s,0,0,1,0,NULL))return s->error;
 uint32_t r7=0;if(simple(s,8,0x1aa,1,4,&r7))return s->error;
 if(r7!=0x1aa)return fail(s,SD_RESPONSE);
 uint64_t start=io.us(io.context);bool done=false;
 for(unsigned n=0;n<10000;n++){
  if(command(s,55,0,0,NULL))return close_card(s,s->error);
  uint32_t app=s->last_r1;int e=close_card(s,0);if(e)return e;
  if(app>1)return fail(s,SD_RESPONSE);
  if(command(s,41,0x40000000,0,NULL))return close_card(s,s->error);
  uint32_t r=s->last_r1;e=close_card(s,0);if(e)return e;
  if(!r){done=true;break;}if(r!=1)return fail(s,SD_RESPONSE);
  if(io.us(io.context)-start>=3000000u)return fail(s,SD_TIMEOUT);
 }
 if(!done)return fail(s,SD_TIMEOUT);
 if(simple(s,58,0,0,4,&s->ocr))return s->error;
 if((s->ocr&0xc0000000u)!=0xc0000000u||!(s->ocr&0x00ff8000u))return fail(s,SD_RESPONSE);
 if(simple(s,59,1,0,0,NULL)||reg_data(s,10,s->cid)||reg_data(s,9,s->csd))return s->error;
 if((s->csd[0]>>6)!=1)return fail(s,SD_RESPONSE);
 uint32_t c=((uint32_t)(s->csd[7]&63)<<16)|(uint32_t)s->csd[8]<<8|s->csd[9];
 uint64_t sectors=((uint64_t)c+1)*1024u;if(!sectors||sectors>UINT32_MAX)return fail(s,SD_RANGE);
 s->sectors=(uint32_t)sectors;s->initialized=true;return 0;
}
int sd_spi_read(SdSpi *s,uint32_t lba,uint8_t out[512])
{
 if(!s||!out||!s->initialized||s->failed)return SD_REFUSED;
 if(lba>=s->sectors)return SD_RANGE;
 int r=command(s,17,lba,0,NULL);if(!r&&s->last_r1)r=fail(s,SD_RESPONSE);
 if(!r)r=data(s,out,512);
 r=close_card(s,r);if(!r)s->reads++;return r;
}
int sd_spi_test_gate(SdSpi *s,uint32_t begin,uint32_t count)
{
 static const uint8_t cid[16]={3,0x53,0x44,0x53,0x44,0x33,0x32,0x47,0x85,0x8b,0xf0,0x92,0x3c,1,0x92,0x7a};
 if(!s||!s->initialized||s->failed)return SD_REFUSED;
 s->write_enabled=false;
 if(s->sectors!=62333952u||memcmp(cid,s->cid,15)||(s->cid[15]&0xfe)!=cid[15])return SD_IDENTITY;
 if(begin!=2048u||!count||count>4096u)return SD_RANGE;
 s->begin_lba=begin;s->limit_lba=begin+count;s->write_enabled=true;return 0;
}
int sd_spi_write(SdSpi *s,uint32_t lba,const uint8_t block[512])
{
 if(!s||!block||!s->write_enabled||!s->initialized||s->failed)return SD_REFUSED;
 /* Sector0 only for the explicit sacrificial raw-layout marker; no arbitrary
  * partition/FAT sectors. The authorized bounded data interval is separate. */
 if(lba!=0&&(lba<s->begin_lba||lba>=s->limit_lba))return SD_RANGE;
 int r=command(s,24,lba,0,NULL);if(!r&&s->last_r1)r=fail(s,SD_RESPONSE);
 uint8_t discard[512];
 if(!r)r=byte(s,0xfe,discard);
 if(!r&&s->io.exchange(s->io.context,block,discard,512))r=fail(s,SD_IO);
 uint16_t crc=sd_crc16(block,512);if(!r)r=byte(s,(uint8_t)(crc>>8),discard);if(!r)r=byte(s,(uint8_t)crc,discard);
 uint8_t response=255;
 if(!r){for(unsigned i=0;i<8;i++){r=byte(s,255,&response);if(r||response!=255)break;}
  if(!r&&(response&31)!=5)r=fail(s,SD_RESPONSE);
 }
 if(!r)r=ready(s,500000,true);
 r=close_card(s,r);
 uint32_t status=0;if(!r)r=simple(s,13,0,0,1,&status);
 if(!r&&status)r=fail(s,SD_RESPONSE);
 if(!r)s->writes++;
 return r;
}

/* After an uncertain write timeout, clock only readiness: no retry, no new CMD.
 * The first fault remains latched and all future writes stay refused. */
int sd_spi_quiesce(SdSpi *s)
{
 if(!s||!s->io.exchange)return SD_REFUSED;
 s->io.select(s->io.context,true);int r=ready(s,5000000u,false);
 return close_card(s,r);
}
