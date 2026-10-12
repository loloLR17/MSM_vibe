set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-L-20261012T195811Z/stage-A-flash.bin 0x08000000 0x8007b90
x/3wx 0x0BFA0700
monitor reset

detach
quit
