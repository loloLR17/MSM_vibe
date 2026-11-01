#include "stream_hash.h"
#include <assert.h>
#include <stdio.h>
int main(int argc,char **argv){assert(argc==2&&sizeof(R3Hash)==112);FILE *f=fopen(argv[1],"rb");assert(f);R3Hash s;assert(fread(&s,1,sizeof s,f)==sizeof s&&fgetc(f)==EOF);fclose(f);assert(s.used<64);uint8_t out[32];r3_hash_finish(&s,out);for(unsigned i=0;i<32;i++)printf("%02x",out[i]);puts("");}
