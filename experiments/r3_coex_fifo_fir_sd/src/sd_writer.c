#include "sd_writer.h"
#include <string.h>
static int fault(SdWriter *w,int error)
{
 if(!w->error)w->error=(uint32_t)error;
 if(!w->card->failed){w->card->error=w->error;w->card->failed=true;if(error==SD_TIMEOUT)w->card->timeouts++;}
 w->active=false;w->card->io.select(w->card->io.context,false);return (int)w->error;
}
static int transfer(SdWriter *w,const uint8_t *tx,uint8_t *rx,uint16_t n)
{return w->card->io.exchange(w->card->io.context,tx,rx,n)?fault(w,SD_IO):0;}
static int byte(SdWriter *w,uint8_t tx,uint8_t *rx){return transfer(w,&tx,rx,1);}
static int cmd(SdWriter *w,uint8_t command,uint32_t arg,uint8_t *r1)
{
 uint8_t p[6]={0x40u|command,(uint8_t)(arg>>24),(uint8_t)(arg>>16),(uint8_t)(arg>>8),(uint8_t)arg,0},rx[6];p[5]=sd_crc7(p,5);
 w->card->last_cmd=command;w->card->last_r1=255;
 if(transfer(w,p,rx,6))return SD_IO;
 for(unsigned i=0;i<8;i++){if(byte(w,255,r1))return SD_IO;if(!(*r1&128))break;}
 w->card->last_r1=*r1;w->card->commands++;
 if(*r1&128)return fault(w,SD_TIMEOUT);
 return *r1?fault(w,SD_RESPONSE):0;
}
int sd_writer_r3_gate(SdSpi *s)
{
 static const uint8_t id[16]={3,0x53,0x44,0x53,0x44,0x33,0x32,0x47,0x85,0x8b,0xf0,0x92,0x3c,1,0x92,0x7b};
 if(!s||!s->initialized||s->failed)return SD_REFUSED;
 s->write_enabled=false;
 if(s->sectors!=62333952u||memcmp(s->cid,id,16))return SD_IDENTITY;
 s->begin_lba=8192;s->limit_lba=40960;s->write_enabled=true;return 0;
}
int sd_writer_begin(SdWriter *w,SdSpi *s,uint32_t lba,const uint8_t b[512])
{
 if(!w||!s||!b||w->active||!s->initialized||s->failed||!s->write_enabled)return SD_REFUSED;
#ifdef R3_DURABLE_CARD_PROFILE
 if(s->begin_lba!=0||s->limit_lba!=62333952u||s->sectors!=62333952u||lba>=s->limit_lba)return SD_RANGE;
#else
 if(s->begin_lba!=8192||s->limit_lba!=40960||lba<s->begin_lba||lba>=s->limit_lba)return SD_RANGE;
#endif
 memset(w,0,sizeof *w);w->card=s;w->data=b;w->lba=lba;w->crc=sd_crc16(b,512);w->active=true;w->state=1;w->begin_us=s->io.us(s->io.context);return 0;
}
int sd_writer_step(SdWriter *w)
{
 if(!w||!w->card)return SD_REFUSED;
 if(w->error)return (int)w->error;
 if(!w->active)return w->done?0:SD_REFUSED;
 SdSpi *s=w->card;uint8_t r=255,rx[128];uint64_t now=s->io.us(s->io.context);
 if(now-w->begin_us>1000000u)return fault(w,SD_TIMEOUT);
 switch(w->state){
 case 1:
  s->io.select(s->io.context,true);
  if(byte(w,255,&r))return w->error;
  if(r!=255)return 0;
  if(cmd(w,24,w->lba,&r))return w->error;
  w->state=2;return 0;
 case 2:
  if(byte(w,0xfe,&r))return w->error;
  w->state=3;return 0;
 case 3:
  if(transfer(w,w->data+w->offset,rx,128))return w->error;
  w->offset+=128;if(w->offset==512)w->state=4;return 0;
 case 4:{
  w->uncertain=true; /* CRC I/O can fail after the card actually accepted data. */
  uint8_t crc[2]={(uint8_t)(w->crc>>8),(uint8_t)w->crc};
  if(transfer(w,crc,rx,2))return w->error;
  for(unsigned i=0;i<8;i++){if(byte(w,255,&r))return w->error;if(r!=255)break;}
  if((r&31)!=5)return fault(w,SD_RESPONSE);
  /* Provide the eight post-response clocks before parking SCK for FIFO work. */
  if(byte(w,255,&r))return w->error;
  w->uncertain=true;w->busy_begin_us=s->io.us(s->io.context);w->state=5;return 0;
 }
 case 5:
  if(byte(w,255,&r))return w->error;
  if(r!=255){if(now-w->busy_begin_us>=750000u)return fault(w,SD_TIMEOUT);return 0;}
  w->busy_us=s->io.us(s->io.context)-w->busy_begin_us;
  /* End the programming transaction before a separate CMD13/R2 status. */
  s->io.select(s->io.context,false);if(byte(w,255,&r))return w->error;w->state=6;return 0;
 case 6:
  s->io.select(s->io.context,true);
  if(byte(w,255,&r))return w->error;
  if(r!=255)return 0;
  if(cmd(w,13,0,&r)||byte(w,255,&r))return w->error;
  if(r)return fault(w,SD_RESPONSE);
  s->io.select(s->io.context,false);if(byte(w,255,&r))return w->error;
  w->complete_us=s->io.us(s->io.context);w->uncertain=false;w->active=false;w->done=true;
  s->writes++;s->last_busy_us=(uint32_t)w->busy_us;if(w->busy_us>s->max_busy_us)s->max_busy_us=(uint32_t)w->busy_us;return 0;
 default:return fault(w,SD_REFUSED);
 }
}
