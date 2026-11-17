set pagination off
set confirm off
set print elements 24
set remotetimeout 10
target extended-remote localhost:23341
x/3wx 0x0BFA0700
if *(unsigned int *)0xBFA0700 != 0x004A003D || *(unsigned int *)0xBFA0704 != 0x38425018 || *(unsigned int *)0xBFA0708 != 0x20333157
 disconnect
 quit 3
end
info registers pc sp lr xpsr
x/8wx 0x08000000
x/wx 0xE000ED08
x/2wx 0xE000ED28
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/001-initial-flash-2MiB.bin 0x08000000 0x08200000
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/001-initial-ram-768KiB.bin 0x20000000 0x200C0000

detach
quit
