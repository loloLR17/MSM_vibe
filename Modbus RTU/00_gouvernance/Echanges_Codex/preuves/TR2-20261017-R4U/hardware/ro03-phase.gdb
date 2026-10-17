set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
monitor reset
shell sleep 1
p r4u_result
if r4u_result.stage != 1 || r4u_result.error || work.error || work.deadline_missed || work.max_work_us > 1000 || work.max_service_us > 2500 || r4_result.max_gap_us > 5000
 quit 4
end
break r4u_uart_checkpoint
break r4u_checkpoint
set r4_trigger=0x52345502
continue
p r4u_result
p g_serial
if r4u_result.error || r4u_result.stage != 4
 quit 6
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4u\hardware\ro03-ready.bin &r4u_result &r4u_result+1
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
p server.captured
p server.queue_full
p server.transport_errors
p tr2_irq_observer
p g_serial
p r4m_range_count
p r4m_ranges
p r4r_first_value
set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4u\hardware\ro03-fram.bin baseline baseline+262144
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4u\hardware\ro03-first-xyz.bin r4r_xyz r4r_xyz+4096
if $rr || r4u_result.error || work.error || work.deadline_missed || work.max_work_us > 1000 || work.max_service_us > 2500 || r4_result.max_gap_us > 5000
 quit 5
end

detach
quit
