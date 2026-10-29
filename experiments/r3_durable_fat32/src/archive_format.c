#include "archive_format.h"
#include <string.h>
uint32_t durable_crc(const uint8_t *b,uint32_t n){uint32_t c=~0u;for(uint32_t i=0;i<n;i++){c^=b[i];for(unsigned j=0;j<8;j++)c=(c>>1)^((c&1)?0xedb88320u:0);}return ~c;}
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
