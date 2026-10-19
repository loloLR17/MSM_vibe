set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if owner.acquisition.fifo.running || engine.has_active || r4u_result.error != 0
 quit 7
end
tbreak u_service
continue
p work
if !work.servicing || owner.acquisition.fifo.running || engine.has_active
 quit 8
end
bt
set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
if $rr != 0
 quit 9
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-live-com4-loss-diag\hardware\reproduction-fram.bin baseline baseline+262144

detach
quit
