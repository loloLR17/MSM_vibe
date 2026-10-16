set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4t_recovery
if r4t_recovery.stage != 1 || r4t_recovery.error
 quit 4
end
break r4r_checkpoint
set r4s_backup_token=0x53345342
set r4_trigger=0x5234000D
continue
p r4t_recovery
p r4_result
p work
p engine
p store
p retirement
p r4m_guard
p owner.acquisition.fifo
set $r=(int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r
if $r != 0
 quit 6
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4t\hardware\fram-after-maintenance.bin baseline baseline+262144

detach
quit
