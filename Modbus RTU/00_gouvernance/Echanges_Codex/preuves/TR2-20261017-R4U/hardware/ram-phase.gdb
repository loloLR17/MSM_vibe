set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if r4u_result.stage != 1 || r4u_result.error
 quit 4
end
break r4u_checkpoint
set r4_trigger=0x52345501
continue
p r4u_result
p r4_result
p work
p runtime
p metric
p event_time
p r4r_result
p r4s_result
p engine
p live.generation
p store.generation
p store.epoch
p store.effect
p owner.acquisition.fifo
set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4u\hardware\ram-fram.bin baseline baseline+262144
if $rr || r4u_result.stage != 3 || r4u_result.error || work.fault || r4_result.max_gap_us > 5000 || r4_result.write_bytes
 quit 5
end

detach
quit
