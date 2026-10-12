set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
x/wx 0xE0044000
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-Q-20261012T224111Z/repeat2-flash.bin 0x08000000 0x800d5fc
p bench_result
x/2wx 0xE000ED28

detach
quit
