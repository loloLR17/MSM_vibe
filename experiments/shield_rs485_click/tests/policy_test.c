#include "policy.h"
#include "tr2/modbus/codec.h"
#include <assert.h>
#include <string.h>
int main(void)
{
    assert(click_mode_allowed(1,0,true));assert(!click_mode_allowed(2,0,true));
    assert(click_mode_allowed(2,CLICK_ARM,true));assert(click_mode_allowed(3,CLICK_ARM,true));
    assert(!click_mode_allowed(3,CLICK_ARM,false));assert(!click_mode_allowed(4,CLICK_ARM,true));
    uint8_t in[14]={'H','1','8','?',1,2,3,4,5,6,7,8},out[26];uint16_t c;uint32_t uid[3]={0x11223344,0x55667788,0x99aabbcc};
    assert(modbus_rtu_crc16(in,12,&c));in[12]=(uint8_t)c;in[13]=(uint8_t)(c>>8);
    assert(click_raw_reply(in,14,uid,out,26)==26);assert(memcmp(in,out,14)!=0);
    assert(!memcmp(out,"T18!",4) && !memcmp(out+4,in+4,8));assert(out[12]==0x44 && out[23]==0x99);
    assert(modbus_rtu_crc16(out,26,&c) && c==0);
    in[13]^=1;assert(!click_raw_reply(in,14,uid,out,26));in[13]^=1;
    assert(!click_raw_reply(in,13,uid,out,26));assert(!click_raw_reply(in,14,uid,out,25));
    in[0]='T';assert(!click_raw_reply(in,14,uid,out,26));
    return 0;
}
