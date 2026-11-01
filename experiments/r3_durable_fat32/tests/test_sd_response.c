/* Exact SD reject/no-response must latch uncertainty and forbid any retry.
 * Synthetic transport responses, never physical SPI qualification. */
#include "sd_group.h"
#include "tr2_001_card.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint8_t token;
static unsigned exchanges;
static bool selected;
static uint64_t us(void *p){(void)p;return 20;}
static void cs(void *p,bool on){(void)p;selected=on;}
static int exchange(void *p,const uint8_t *tx,uint8_t *rx,uint16_t n)
{
 (void)p;exchanges++;memset(rx,255,n);
 if(exchanges==1){assert(n==2&&tx[0]==0xf4&&tx[1]==0x24);}
 else{assert(n==1&&tx[0]==255);if(exchanges==2)rx[0]=token;}
 return 0;
}
static void run(uint8_t response,bool accept)
{
 uint8_t data[512]={0};SdSpi s={0};SdGroup w={0};
 s.io=(SdSpiIo){NULL,exchange,cs,us};s.initialized=s.write_enabled=true;
 s.sectors=s.limit_lba=TR2_001_CARD_SECTORS;memcpy(s.cid,tr2_001_card_cid,16);
 w.card=&s;w.active=w.uncertain=true;w.state=6;w.blocks=8;w.block=1;w.offset=512;
 w.crc=62500;w.deadline_us=1000000;selected=true;exchanges=0;token=response;
 int r=sd_group_step(&w);
 assert(w.data_response_seen&&w.data_response==response);
 if(accept){assert(!r&&w.active&&!w.done&&!w.needs_power_cycle&&w.state==7&&!s.failed);}
 else{
  assert(r==SD_RESPONSE&&s.failed&&!w.active&&!w.done&&w.uncertain&&w.needs_power_cycle&&!selected);
  unsigned before=exchanges;
  assert(sd_group_step(&w)==SD_RESPONSE);
  assert(sd_group_begin(&w,&s,2048,data,1)==SD_REFUSED);
  assert(exchanges==before); /* No guessed stop, extra command or data retry. */
 }
}
int main(void){run(0x05,true);run(0xe5,true);run(0x0b,false);run(0x0d,false);run(0x00,false);run(0xff,false);puts("PASS accepted/reject/CRC-error/write-error/missing token; uncertainty/no-retry retained (MODEL)");}
