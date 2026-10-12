set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
x/wx 0xE0044000
x/2wx 0xE000ED28
info registers pc
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-L-attempt02-20261012T201926Z/uid.bin 0x0BFA0700 0x0BFA070C
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-L-attempt02-20261012T201926Z/flash-before-first.bin 0x08000000 0x08200000
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-L-attempt02-20261012T201926Z/ram-before.bin 0x20000000 0x200C0000

detach
quit
