#include "policy.h"
#include "tr2/modbus/codec.h"
#include <string.h>
bool click_mode_allowed(uint32_t cmd,uint32_t arm,bool low)
{
    return low && cmd<=3u && (cmd<=1u || arm==CLICK_ARM);
}
size_t click_raw_reply(const uint8_t *in,size_t n,const uint32_t uid[3],uint8_t *out,size_t cap)
{
    uint16_t crc;
    if(!in || !uid || !out || n!=14u || cap<26u || memcmp(in,"H18?",4))return 0;
    if(!modbus_rtu_crc16(in,12u,&crc) || in[12]!=(uint8_t)crc || in[13]!=(uint8_t)(crc>>8))return 0;
    memcpy(out,"T18!",4);memcpy(out+4,in+4,8);
    for(unsigned i=0;i<3;i++)for(unsigned j=0;j<4;j++)out[12+4*i+j]=(uint8_t)(uid[i]>>(8*j));
    if(!modbus_rtu_crc16(out,24u,&crc))return 0;
    out[24]=(uint8_t)crc;out[25]=(uint8_t)(crc>>8);return 26;
}
