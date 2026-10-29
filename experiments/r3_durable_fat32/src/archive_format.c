#include "archive_format.h"
#include <string.h>
/* Same IEEE CRC32, nibble table bounds callback cost; Python zlib is oracle. */
uint32_t durable_crc(const uint8_t *b,uint32_t n){static const uint32_t t[16]={0,0x1db71064,0x3b6e20c8,0x26d930ac,0x76dc4190,0x6b6b51f4,0x4db26158,0x5005713c,0xedb88320,0xf00f9344,0xd6d6a3e8,0xcb61b38c,0x9b64c2b0,0x86d3d2d4,0xa00ae278,0xbdbdf21c};uint32_t c=~0u;for(uint32_t i=0;i<n;i++){c^=b[i];c=(c>>4)^t[c&15];c=(c>>4)^t[c&15];}return ~c;}
void durable_put32(uint8_t *b,uint32_t n){for(unsigned i=0;i<4;i++)b[i]=(uint8_t)(n>>(i*8));}
void durable_put64(uint8_t *b,uint64_t n){for(unsigned i=0;i<8;i++)b[i]=(uint8_t)(n>>(i*8));}
static uint32_t get32(const uint8_t *b){return (uint32_t)b[0]|(uint32_t)b[1]<<8|(uint32_t)b[2]<<16|(uint32_t)b[3]<<24;}
void durable_record(uint8_t b[512],uint64_t session,uint32_t seq,uint32_t index,uint64_t first,uint64_t last,uint16_t count,uint16_t flags)
{
 memset(b,0,44);memcpy(b,"D3BK",4);durable_put32(b+4,seq);durable_put64(b+8,session);durable_put64(b+16,first);durable_put64(b+24,last);durable_put32(b+32,index);b[36]=(uint8_t)count;b[37]=(uint8_t)(count>>8);b[38]=(uint8_t)flags;b[39]=(uint8_t)(flags>>8);
 if(count<DURABLE_SAMPLES)memset(b+44+count*6,0,468-count*6);
 durable_put32(b+40,durable_crc(b,512));
}
bool durable_record_valid(const uint8_t b[512],uint64_t session,uint32_t seq,uint32_t index)
{
 uint8_t tmp[512],id[8];durable_put64(id,session);if(memcmp(b,"D3BK",4)||memcmp(b+8,id,8)||get32(b+4)!=seq||get32(b+32)!=index||!b[36]||b[36]>78||b[37])return false;
 memcpy(tmp,b,512);memset(tmp+40,0,4);return get32(b+40)==durable_crc(tmp,512);
}
