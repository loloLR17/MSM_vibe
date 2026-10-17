set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4u\hardware\image02-readback.bin 0x08000000 0x08000000+146104
monitor reset
shell sleep 1
p r4u_result
if r4u_result.stage != 1 || r4u_result.error != 0
 quit 4
end

detach
quit
