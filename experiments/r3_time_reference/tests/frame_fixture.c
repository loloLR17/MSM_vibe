#include "reference_frame.h"
#include <stdio.h>
#include "witness_uart.h"
int main(void){uint8_t b[32];uint32_t uid[3]={0x001b0046,0x38425018,0x20333157};reference_frame(b,uid,7,10000000000ull);if(fwrite(b,1,32,stdout)!=32)return 1;uint8_t a[48];witness_frame(a,uid,3,5000000,1|(5u<<8),99,1234);return fwrite(a,1,48,stdout)==48?0:1;}
