set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p mapper
p bench_trace_count
p bench_payload_count
p bench_preclose_count
p bench_preclose_overflow
p bench_log_refused
p q.head
p q.tail
p q.count
p q.borrowed
p durable_frame_count
p durable_frame_seq
p durable_first_output
p frame_first_time
p frame_last_time
p fifo.running
p fifo.stopped
p r3_writer.active
p r3_writer.uncertain
p r3_writer.needs_power_cycle
p/x *(unsigned int *)0x42021414
if fifo.running || !fifo.stopped || r3_writer.active || r3_writer.uncertain || r3_writer.needs_power_cycle || !(*(unsigned int *)0x42021414 &0x1000)
 quit 12
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\mapper.bin &mapper (char *)&mapper+sizeof(mapper)

detach
quit
