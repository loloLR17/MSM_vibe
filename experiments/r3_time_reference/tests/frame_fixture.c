#include "reference_frame.h"
#include <stdio.h>
int main(void){uint8_t b[32];uint32_t uid[3]={0x001b0046,0x38425018,0x20333157};reference_frame(b,uid,7,10000000000ull);return fwrite(b,1,32,stdout)==32?0:1;}
