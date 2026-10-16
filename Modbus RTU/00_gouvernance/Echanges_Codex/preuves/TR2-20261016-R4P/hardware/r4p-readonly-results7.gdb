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
p r4m_guard
p r4m_protected_bytes
p r4m_protected_checks
p engine
p store.epoch
p store.effect
p owner.acquisition.fifo
if r4_result.stage != 10 || r4_result.error || r4_result.write_bytes || work.error || work.deadline_missed || work.max_work_us > 1000 || work.max_service_us > 2500 || r4m_protected_bytes != 139264 || owner.acquisition.fifo.running || engine.indeterminate || engine.has_active || r4m_guard.phase != 0
 quit 4
end
set $r=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r
if $r != 0
 quit 5
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4p\hardware\fram-after-readonly.bin baseline baseline+262144

detach
quit
