set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result.stage
p r3_result.error
p fifo.running
p fifo.stopped
p r3_writer.active
p r3_card.commands
p r3_card.reads
p r3_card.writes
p r3_card.initialized
p r3_card.write_enabled
p/x *(unsigned int *)0x42021414
p r3_retained_available
p/x r3_retained.guard
if r3_result.stage!=4 || r3_result.error || fifo.running || !fifo.stopped || r3_writer.active || r3_card.commands || r3_card.reads || r3_card.writes || r3_card.initialized || r3_card.write_enabled
 quit 12
end

detach
quit
