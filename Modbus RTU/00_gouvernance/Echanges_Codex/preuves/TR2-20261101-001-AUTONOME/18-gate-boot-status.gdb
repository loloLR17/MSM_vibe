set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result
p standalone_state
p standalone_fs_readback
p durable_status
p fs
p r3_card.error
p r3_card.reads
p r3_card.writes
p fifo.running
p r3_clock_fault
bt 5

detach
quit
