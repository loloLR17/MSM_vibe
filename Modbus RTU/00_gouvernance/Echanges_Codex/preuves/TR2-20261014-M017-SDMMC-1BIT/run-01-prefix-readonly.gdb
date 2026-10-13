set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if load_stage != 99 || load_producer_enabled != 0 || load_written != 65536
 quit 4
end
set $ok=transfer(0,rx,2048,65536)
printf "READ offset=0 bytes=65536 status=%u\n", $ok
if $ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M017-20261014\run-01-prefix-0000.bin rx rx+65536
p load_stage
p load_error
p/x hsd2.ErrorCode
p/x load_last_r1
x/2wx 0xE000ED28

detach
quit
