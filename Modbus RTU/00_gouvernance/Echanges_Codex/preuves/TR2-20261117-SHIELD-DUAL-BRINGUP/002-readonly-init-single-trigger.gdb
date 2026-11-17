set pagination off
set confirm off
set print elements 24
set remotetimeout 10
target extended-remote localhost:23341
x/3wx 0x0BFA0700
if *(unsigned int *)0xBFA0700 != 0x001B0046 || *(unsigned int *)0xBFA0704 != 0x38425018 || *(unsigned int *)0xBFA0708 != 0x20333157
 disconnect
 quit 3
end
p r3_result
if r3_result.stage != 2 || r3_result.run != 0 || r3_command != 0 || fifo.running || r3_writer.active || r3_writer.uncertain || r3_writer.needs_power_cycle
 disconnect
 quit 12
end
set r3_command = 0x44335201

detach
quit
