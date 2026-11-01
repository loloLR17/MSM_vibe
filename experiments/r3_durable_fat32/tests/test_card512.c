#include "tr2_001_card.h"
#include <assert.h>
#include <stdio.h>
static void p32(uint8_t *b,uint32_t v){for(unsigned i=0;i<4;i++)b[i]=(uint8_t)(v>>(8*i));}
int main(void){
 uint8_t m[512]={0},b[512]={0};uint32_t start=0,length=0,cap=1000215216u;
 m[510]=b[510]=0x55;m[511]=b[511]=0xaa;m[450]=0x0c;p32(m+454,2048);p32(m+458,cap-2048);
 assert(tr2_001_partition(m,cap,&start,&length)&&start==2048&&length==cap-2048);
 b[12]=2;b[13]=64;b[14]=32;b[16]=2;p32(b+28,start);p32(b+32,length);p32(b+36,123456);p32(b+44,2);
 assert(tr2_001_bpb(b,start,length));
 b[12]=4;assert(!tr2_001_bpb(b,start,length));b[12]=2;
 b[13]=3;assert(!tr2_001_bpb(b,start,length));b[13]=64;
 p32(b+32,length+1);assert(!tr2_001_bpb(b,start,length));
 p32(m+458,0xffffffffu);assert(!tr2_001_partition(m,cap,&start,&length));p32(m+458,cap-2048);
 m[466]=0x0c;assert(!tr2_001_partition(m,cap,&start,&length));m[466]=0;
 m[450]=7;assert(!tr2_001_partition(m,cap,&start,&length));
 uint8_t wrong[16]={3,0x53,0x44,0x53,0x44,0x33,0x32,0x47,0x85,0x8b,0xf0,0x92,0x3c,1,0x92,0x7b};
 assert(!tr2_001_card_matches(wrong,62333952u));
 if(TR2_001_CARD_SECTORS){assert(tr2_001_card_matches(tr2_001_card_cid,TR2_001_CARD_SECTORS));}
 else{uint8_t zero[16]={0};assert(!tr2_001_card_matches(zero,0));}
 puts("PASS FAT32 sector/cluster/MBR/range/overflow/exFAT/old-card fail-closed (MODEL)");
}
