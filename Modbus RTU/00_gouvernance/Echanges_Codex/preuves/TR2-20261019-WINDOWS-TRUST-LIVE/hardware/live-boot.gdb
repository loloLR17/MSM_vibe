set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4u_result
p owner.acquisition.fifo
p engine
p store
p live
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-windows-trust-live-resume\hardware\live-readback.bin 0x08000000 0x8023e50

detach
quit
