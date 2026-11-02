set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
set print elements 16
p r3_result
p fifo.running
p r3_writer.active
p r3_card.writes
if fifo.running || r3_writer.active || r3_card.writes
 quit 12
end
if standalone_state==1
 thbreak Error_Handler
 thbreak main.c:440
 continue
 delete breakpoints
end
p r3_result
p standalone_state
p fifo.running
p r3_writer.active
p r3_writer.uncertain
p r3_writer.needs_power_cycle
p r3_card.writes
p/x *(unsigned int *)0x42021414
p/x r3_retained.guard

detach
quit
