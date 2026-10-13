set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p load_stage
p/x load_cid
p load_blocks
p load_clock_hz
p/x load_clkcr
p load_bus_card
p/x GPIOB->MODER
if load_stage != 2 || load_clock_hz != 10000000 || load_bus_card != 0 || load_producer_enabled != 0 || (GPIOB->MODER & 0xc00003c0) != 0
 quit 4
end
set $ok=transfer(0,rx,2048,65536)
p $ok
if $ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M017-20261014\run-01-prefix-after-reinit.bin rx rx+65536
p/x hsd2.ErrorCode
p/x load_last_r1
x/2wx 0xE000ED28

detach
quit
