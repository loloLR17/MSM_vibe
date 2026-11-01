/* TR2-001 only: release-to-arm, debounce, deliberate one-shot START. */
#ifndef R3_STANDALONE_H
#define R3_STANDALONE_H
#include <stdbool.h>
#include <stdint.h>
typedef struct {bool raw,stable,armed,started;uint64_t changed,pressed;} StandaloneButton;
static inline bool standalone_button(StandaloneButton *b,bool high,uint64_t ms)
{
 if(high!=b->raw){b->raw=high;b->changed=ms;}
 if(ms-b->changed<50)return false;
 if(b->stable!=high){b->stable=high;if(high)b->pressed=ms;}
 if(!high){b->armed=true;return false;}
 if(b->armed&&!b->started&&ms-b->pressed>=1500){b->started=true;return true;}
 return false;
}
#endif
