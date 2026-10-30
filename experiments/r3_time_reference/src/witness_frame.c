#include "witness_uart.h"
#include "archive_format.h"
#include <string.h>
void witness_frame(uint8_t b[48],const uint32_t uid[3],uint32_t seq,uint64_t us,uint32_t flags,uint64_t session,uint32_t accepted)
{
 memcpy(b,"QPC2",4);for(unsigned j=0;j<3;j++)durable_put32(b+4+4*j,uid[j]);
 durable_put32(b+16,seq);durable_put64(b+20,us);durable_put32(b+28,flags);
 durable_put64(b+32,session);durable_put32(b+40,accepted);durable_put32(b+44,durable_crc(b,44));
}
