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
p r3_clock_fault
p durable_status
p r3_card
p r3_writer
p fifo.running
p r3_irqs
p/x *(unsigned int *)0x42021814
p/x *(unsigned int *)0x40003808
p/x *(unsigned int *)0xE000EDFC
p/x *(unsigned int *)0xE0001000
p/x *(unsigned int *)0xE0001004
p/x *(unsigned int *)0x46020c94
bt

detach
quit
