/* Exact TR2-001 card, observed after CLK replacement: 15-clk-replaced-reconnected.
 * CID/CSD/sector0 CRC checked, 999743488 sectors; never a capacity-only fallback. */
#ifndef TR2_001_CARD_H
#define TR2_001_CARD_H
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#define TR2_001_CARD_SECTORS 999743488u
static const uint8_t tr2_001_card_cid[16]={0x03,0x53,0x44,0x53,0x44,0x35,0x31,0x32,0x85,0x62,0xe4,0x3c,0xf8,0x01,0x95,0xeb};
static inline bool tr2_001_card_matches(const uint8_t cid[16],uint32_t sectors)
{return TR2_001_CARD_SECTORS && sectors==TR2_001_CARD_SECTORS && !memcmp(cid,tr2_001_card_cid,16);}
static inline uint32_t card_u32(const uint8_t *b)
{return (uint32_t)b[0]|(uint32_t)b[1]<<8|(uint32_t)b[2]<<16|(uint32_t)b[3]<<24;}
/* Single ordinary FAT32 MBR partition, no protective GPT/exFAT/superfloppy. */
static inline bool tr2_001_partition(const uint8_t m[512],uint32_t sectors,uint32_t *start,uint32_t *length)
{
 if(m[510]!=0x55||m[511]!=0xaa)return false;
 unsigned found=0;
 for(unsigned i=0;i<4;i++){
  const uint8_t *p=m+446+16*i;
  if(!p[4]){if(card_u32(p+8)||card_u32(p+12))return false;continue;}
  if(++found!=1||(p[0]!=0&&p[0]!=128)||(p[4]!=0x0b&&p[4]!=0x0c))return false;
  *start=card_u32(p+8);*length=card_u32(p+12);
  if(*start<2048||!*length||*start>=sectors||*length>sectors-*start)return false;
 }
 return found==1;
}
static inline bool tr2_001_bpb(const uint8_t b[512],uint32_t start,uint32_t length)
{
 uint32_t cluster=b[13],total=card_u32(b+32),fat=card_u32(b+36);
 return b[510]==0x55&&b[511]==0xaa&&b[11]==0&&b[12]==2&&cluster&&cluster<=128&&!(cluster&(cluster-1))&&
  (b[14]||b[15])&&(b[16]==1||b[16]==2)&&!b[17]&&!b[18]&&!b[19]&&!b[20]&&!b[22]&&!b[23]&&
  card_u32(b+28)==start&&total&&total<=length&&fat&&card_u32(b+44)>=2;
}
#endif
