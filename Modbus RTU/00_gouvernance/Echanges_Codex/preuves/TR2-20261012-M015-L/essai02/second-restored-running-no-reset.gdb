set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
info registers pc
x/3wx 0x0BFA0700
x/2wx 0xE000ED28
x/wx 0xE000ED08

detach
quit
