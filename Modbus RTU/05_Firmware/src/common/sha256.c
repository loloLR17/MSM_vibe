#include "tr2/common/sha256.h"
#include <string.h>

/* FIPS 180-4, sections 4.1.2, 4.2.2, 5.1.1 and 6.2. No heap or HAL. */
static const uint32_t constants[64] = {
 0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
 0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
 0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
 0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
 0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
 0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
 0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
 0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u
};
static uint32_t rotate(uint32_t x, unsigned n) {return (x >> n) | (x << (32u-n));}
static uint32_t read_be(const uint8_t *p)
{return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];}
static void block(uint32_t h[8], const uint8_t data[64])
{
    uint32_t w[64], a,b,c,d,e,f,g,k;
    unsigned i;
    for (i=0u;i<16u;++i) w[i]=read_be(data+4u*i);
    for (i=16u;i<64u;++i) {
        uint32_t x=w[i-15u], y=w[i-2u];
        w[i]=w[i-16u]+(rotate(x,7)^rotate(x,18)^(x>>3))+
             w[i-7u]+(rotate(y,17)^rotate(y,19)^(y>>10));
    }
    a=h[0];b=h[1];c=h[2];d=h[3];e=h[4];f=h[5];g=h[6];k=h[7];
    for (i=0u;i<64u;++i) {
        uint32_t t1=k+(rotate(e,6)^rotate(e,11)^rotate(e,25))+
                    ((e&f)^((~e)&g))+constants[i]+w[i];
        uint32_t t2=(rotate(a,2)^rotate(a,13)^rotate(a,22))+
                    ((a&b)^(a&c)^(b&c));
        k=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;
    }
    h[0]+=a;h[1]+=b;h[2]+=c;h[3]+=d;h[4]+=e;h[5]+=f;h[6]+=g;h[7]+=k;
}
Tr2Result tr2_sha256_bytes(const uint8_t *data,size_t size,uint8_t digest[32])
{
    uint32_t h[8]={0x6a09e667u,0xbb67ae85u,0x3c6ef372u,0xa54ff53au,
                   0x510e527fu,0x9b05688cu,0x1f83d9abu,0x5be0cd19u};
    uint8_t tail[128]={0};
    uint64_t bits;
    size_t remaining=size, padded, i;
    if ((data==NULL && size!=0u)||digest==NULL) return TR2_ERROR_INVALID_ARGUMENT;
#if SIZE_MAX > (UINT64_MAX / 8u)
    if(size>UINT64_MAX/8u) return TR2_ERROR_INVALID_ARGUMENT;
#endif
    bits=(uint64_t)size*8u;
    while (remaining>=64u) {block(h,data);data+=64u;remaining-=64u;}
    if (remaining!=0u) memcpy(tail,data,remaining);
    tail[remaining]=0x80u;
    padded=remaining<56u?64u:128u;
    for(i=0u;i<8u;++i) tail[padded-1u-i]=(uint8_t)(bits>>(8u*i));
    block(h,tail);
    if(padded==128u) block(h,tail+64u);
    for(i=0u;i<32u;++i) digest[i]=(uint8_t)(h[i/4u]>>(24u-8u*(i%4u)));
    return TR2_OK;
}
