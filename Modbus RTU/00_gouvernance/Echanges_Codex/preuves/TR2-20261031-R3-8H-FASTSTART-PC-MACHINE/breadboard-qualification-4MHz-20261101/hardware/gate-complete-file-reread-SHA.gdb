set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result
if r3_result.stage!=4 || r3_result.run!=3 || r3_result.error || r3_clock_fault || r3_command || fifo.running || r3_writer.active || q.count
 quit 4
end
set $verified = (int)durable_verify_last(r3_source_sha)
p $verified
p durable_status
if $verified || durable_status.error || durable_status.io_uncertain || !(*(unsigned int*)0x42021414 &0x1000)
 quit 5
end
p r3_source_sha

detach
quit
