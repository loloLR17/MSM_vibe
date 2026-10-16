set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4_result
p owner.acquisition.fifo.running
set $r4_stop=(int)fifo_live_stop(&owner.acquisition.fifo)
p $r4_stop
if $r4_stop != 0
 quit 7
end
set var fram.timeout_ms=10
set $r4_control=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r4_control
if $r4_control != 0
 quit 8
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4\hardware\fram-after-r2-indeterminate.bin baseline baseline+262144

detach
quit
