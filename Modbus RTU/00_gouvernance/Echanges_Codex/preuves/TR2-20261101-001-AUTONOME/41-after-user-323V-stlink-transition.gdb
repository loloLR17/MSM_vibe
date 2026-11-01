set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p/x *(unsigned int *)0x46020cf4
p r3_irqs
p r3_result
p standalone_state
p r3_clock_fault
p r3_card.error
p r3_card.commands
p r3_card.last_cmd
p r3_card.last_r1
p r3_card.reads
p r3_card.writes
p r3_writer.active
p fifo.running
p/x *(unsigned int *)0x42021814
bt

detach
quit
