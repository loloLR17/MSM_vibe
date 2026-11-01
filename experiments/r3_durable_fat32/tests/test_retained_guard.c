#include "retained_guard.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
 RetainedGuard g={0};uint8_t p[512],saved[512];memset(p,0x5a,sizeof p);
 assert(!retained_valid(&g,p,sizeof p));retained_seal(&g,p,sizeof p);
 assert(retained_valid(&g,p,sizeof p));memcpy(saved,p,sizeof p);
 /* Reset of ordinary startup state must not be mistaken for a valid capture. */
 RetainedGuard boot={0};assert(!retained_valid(&boot,p,sizeof p));
 for(unsigned i=0;i<sizeof p;i++){p[i]^=1;assert(!retained_valid(&g,p,sizeof p));p[i]^=1;}
 assert(!retained_valid(&g,p,sizeof p-1));g.version=2;assert(!retained_valid(&g,p,sizeof p));g.version=1;
 g.crc^=1;assert(!retained_valid(&g,p,sizeof p));g.crc^=1;
 g.magic=0;memcpy(p,saved,sizeof p);assert(!retained_valid(&g,p,sizeof p));
 retained_seal(&g,p,sizeof p);assert(retained_valid(&g,p,sizeof p));
 puts("PASS incomplete commit, payload corruption, layout/version/size and reseal; RAM persistence requires target evidence");
}
