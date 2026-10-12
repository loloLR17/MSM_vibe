set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-L-attempt02-20261012T201926Z/flash-before-second.bin 0x08000000 0x08200000

detach
quit
