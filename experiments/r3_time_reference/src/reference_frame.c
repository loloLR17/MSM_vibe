#include "reference_frame.h"
#include "archive_format.h"
#include <string.h>
void reference_frame(uint8_t b[32],const uint32_t uid[3],uint32_t seq,uint64_t cycles)
{
 memcpy(b,"QPC1",4);for(unsigned j=0;j<3;j++)durable_put32(b+4+4*j,uid[j]);
 durable_put32(b+16,seq);durable_put64(b+20,cycles);durable_put32(b+28,durable_crc(b,28));
}
