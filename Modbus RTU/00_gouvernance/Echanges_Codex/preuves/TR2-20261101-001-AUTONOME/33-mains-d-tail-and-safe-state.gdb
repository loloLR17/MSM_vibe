set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p durable_frame_count
p durable_frame_seq
p durable_first_output
p q.head
p q.tail
p q.count
p q.borrowed
p source_hash.bytes
p r3_writer.state
p r3_writer.error
p r3_writer.block
p r3_writer.crc
p r3_writer.needs_power_cycle
p fifo.stopped
p r3_writer.active
p/x *(unsigned int *)0x42021414

detach
quit
