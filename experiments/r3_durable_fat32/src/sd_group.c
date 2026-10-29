#include "sd_group.h"
#include <string.h>
static int fault(SdGroup *w,int e)
{
 if(!w->error)w->error=(uint32_t)e;
 w->card->failed=true;w->card->error=w->error;if(e==SD_TIMEOUT)w->card->timeouts++;w->active=false;
 /* A CMD25 data/CRC I/O failure has unknown protocol position. Do not send a
  * guessed stop/retry/CMD13 into that stream: source STOP, require power cycle. */
 if(w->state>=3&&w->state<=9)w->needs_power_cycle=true;
 w->card->io.select(w->card->io.context,false);return e;
}
static int tx(SdGroup *w,const uint8_t *b,uint8_t *r,uint16_t n){return w->card->io.exchange(w->card->io.context,b,r,n)?fault(w,SD_IO):0;}
static int byte(SdGroup *w,uint8_t b,uint8_t *r){return tx(w,&b,r,1);}
static int cmd(SdGroup *w,uint8_t cmd,uint32_t arg)
{
 uint8_t b[6]={0x40u|cmd,(uint8_t)(arg>>24),(uint8_t)(arg>>16),(uint8_t)(arg>>8),(uint8_t)arg,0},r[6];b[5]=sd_crc7(b,5);
 if(tx(w,b,r,6))return w->error;
 uint8_t v=255;for(unsigned i=0;i<8;i++){if(byte(w,255,&v))return w->error;if(!(v&128))break;}
 w->card->last_cmd=cmd;w->card->last_r1=v;w->card->commands++;
 return v?fault(w,(v&128)?SD_TIMEOUT:SD_RESPONSE):0;
}
int sd_group_begin(SdGroup *w,SdSpi *s,uint32_t lba,const uint8_t *data,uint32_t blocks)
{
 if(!w||w->active||!s||!data||!s->initialized||s->failed||!s->write_enabled)return SD_REFUSED;
 if(s->sectors!=62333952u||s->begin_lba||s->limit_lba!=s->sectors||!blocks||blocks>8||lba>=s->sectors||blocks>s->sectors-lba)return SD_RANGE;
 memset(w,0,sizeof *w);w->card=s;w->data=data;w->lba=lba;w->blocks=blocks;w->state=1;w->active=true;w->begin_us=s->io.us(s->io.context);w->deadline_us=w->begin_us+1000000;return 0;
}
static int wait_ready(SdGroup *w)
{
 uint8_t r;if(byte(w,255,&r))return -1;
 if(r!=255){if(w->card->io.us(w->card->io.context)-w->busy_begin_us>=750000){fault(w,SD_TIMEOUT);return -1;}return 0;}
 uint32_t busy=(uint32_t)(w->card->io.us(w->card->io.context)-w->busy_begin_us);w->block_busy_us[w->state==9?8:w->block]=busy;w->busy_sum_us+=busy;w->card->last_busy_us=busy;if(busy>w->card->max_busy_us)w->card->max_busy_us=busy;if(busy>w->max_busy_us)w->max_busy_us=busy;return 1;
}
int sd_group_step(SdGroup *w)
{
 if(!w||!w->card)return SD_REFUSED;
 if(w->error)return (int)w->error;
 if(!w->active)return w->done?0:SD_REFUSED;
 if(w->card->io.us(w->card->io.context)>w->deadline_us)return fault(w,SD_TIMEOUT);
 uint8_t r=255,rx[128];int ready;
 switch(w->state){
 case 1:w->card->io.select(w->card->io.context,true);if(byte(w,255,&r))return w->error;if(r!=255)return 0;if(cmd(w,55,0))return w->error;w->state=2;return 0;
 case 2:if(byte(w,255,&r))return w->error;if(cmd(w,23,w->blocks))return w->error;w->card->io.select(w->card->io.context,false);if(byte(w,255,&r))return w->error;w->state=3;return 0;
 case 3:w->card->io.select(w->card->io.context,true);if(byte(w,255,&r))return w->error;if(r!=255)return 0;if(cmd(w,25,w->lba))return w->error;w->state=4;return 0;
 case 4:w->crc=sd_crc16(w->data+w->block*512,512);if(byte(w,255,&r)||byte(w,0xfc,&r))return w->error;w->state=5;w->uncertain=true;return 0;
 case 5:if(tx(w,w->data+w->block*512+w->offset,rx,128))return w->error;w->offset+=128;if(w->offset==512)w->state=6;return 0;
 case 6:{uint8_t crc[2]={(uint8_t)(w->crc>>8),(uint8_t)w->crc};if(tx(w,crc,rx,2))return w->error;
  for(unsigned i=0;i<8;i++){if(byte(w,255,&r))return w->error;if(r!=255)break;}if((r&31)!=5)return fault(w,SD_RESPONSE);
  if(byte(w,255,&r))return w->error;
  w->busy_begin_us=w->card->io.us(w->card->io.context);w->state=7;return 0;}
 case 7:ready=wait_ready(w);if(ready<0)return w->error;if(!ready)return 0;w->block++;w->offset=0;w->deadline_us=w->card->io.us(w->card->io.context)+1000000;w->state=w->block<w->blocks?4:8;return 0;
 case 8:if(byte(w,255,&r)||byte(w,0xfd,&r)||byte(w,255,&r))return w->error;w->busy_begin_us=w->card->io.us(w->card->io.context);w->state=9;return 0;
 case 9:ready=wait_ready(w);if(ready<0)return w->error;if(!ready)return 0;w->card->io.select(w->card->io.context,false);if(byte(w,255,&r))return w->error;w->state=10;return 0;
 case 10:w->card->io.select(w->card->io.context,true);if(byte(w,255,&r))return w->error;if(r!=255)return 0;if(cmd(w,13,0)||byte(w,255,&r))return w->error;if(r)return fault(w,SD_RESPONSE);
  w->card->io.select(w->card->io.context,false);if(byte(w,255,&r))return w->error;w->complete_us=w->card->io.us(w->card->io.context);w->active=false;w->done=true;w->uncertain=false;w->card->writes+=w->blocks;return 0;
 default:return fault(w,SD_REFUSED);
 }
}
