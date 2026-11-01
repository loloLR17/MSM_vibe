set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result.stage
p r3_result.error
p r3_result.run
p fifo.running
p r3_writer.active
p r3_card.writes
p r3_clock_fault
p/x *(unsigned int*)0x42021414
if fifo.running || r3_writer.active || r3_command || r3_clock_fault || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end

detach
quit
