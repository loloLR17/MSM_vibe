set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result
p standalone_fs_readback
p r3_card.writes
p r3_retained_available
p sizeof(r3_retained)
p sizeof(r3_retained.payload)
p/x &r3_retained
p/x &__tr2_retained_end__
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\clockdiag-a1g-readback\flash.bin 0x08000000 0x800cf3c

detach
quit
