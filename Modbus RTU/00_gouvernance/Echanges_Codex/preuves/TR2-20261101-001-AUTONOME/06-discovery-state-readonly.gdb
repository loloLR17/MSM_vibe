set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if r3_result.uid[0] != 0x004a003d || r3_result.uid[1] != 0x38425018 || r3_result.uid[2] != 0x20333157
 quit 4
end
p r3_result
p r3_card
x/512bx r3_media_header

detach
quit
