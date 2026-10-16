set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4_result
p work
p engine.indeterminate
p engine.status
p engine.has_active
p engine.active
p owner.acquisition.fifo.stopped
if r4_result.stage != 10 || r4_result.error != 0 || r4_result.write_bytes != 0 || engine.indeterminate != 1 || engine.status != 9 || r4_result.protected_equal != 1
 quit 9
end
set var fram.timeout_ms=10
set $r4_verify=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r4_verify
if $r4_verify != 0
 quit 8
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4\hardware\fram-after-readonly-recovery.bin baseline baseline+262144

detach
quit
