#include "standalone.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 StandaloneButton b={0};unsigned starts=0;
 /* Held at power-on never arms; release and bounce cannot trigger. */
 for(unsigned t=0;t<3000;t++)starts+=standalone_button(&b,true,t);
 assert(!starts&&!b.armed);
 for(unsigned t=3000;t<4000;t++)starts+=standalone_button(&b,t%7==0,t);
 assert(!starts);
 for(unsigned t=4000;t<4100;t++)starts+=standalone_button(&b,false,t);
 assert(b.armed);
 for(unsigned t=4100;t<5600;t++)starts+=standalone_button(&b,true,t);
 assert(!starts);
 for(unsigned t=5600;t<8000;t++)starts+=standalone_button(&b,true,t);
 assert(starts==1);
 for(unsigned t=8000;t<8200;t++)starts+=standalone_button(&b,false,t);
 for(unsigned t=8200;t<11000;t++)starts+=standalone_button(&b,true,t);
 assert(starts==1);puts("PASS release/debounce/hold/one-shot/repeated-press");
}
