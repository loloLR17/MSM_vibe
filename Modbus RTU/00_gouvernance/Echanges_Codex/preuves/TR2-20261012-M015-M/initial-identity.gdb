set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
x/wx 0xE0044000
x/2wx 0xE000ED28
x/8wx 0x08000000
detach
quit
