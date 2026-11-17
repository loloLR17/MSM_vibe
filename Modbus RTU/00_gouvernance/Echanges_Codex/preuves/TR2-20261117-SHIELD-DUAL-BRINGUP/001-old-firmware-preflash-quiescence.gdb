set pagination off
set confirm off
set print elements 24
set remotetimeout 10
target extended-remote localhost:23341
x/3wx 0x0BFA0700
if *(unsigned int *)0xBFA0700 != 0x004A003D || *(unsigned int *)0xBFA0704 != 0x38425018 || *(unsigned int *)0xBFA0708 != 0x20333157
 disconnect
 quit 3
end
p r3_result
p r3_command
p fifo.running
p r3_writer
p r3_card
p durable_status
if r3_command != 0 || fifo.running || r3_writer.active || r3_writer.uncertain || r3_result.run
 disconnect
 quit 4
end

detach
quit
