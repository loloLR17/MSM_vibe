set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
set print elements 24
p r3_result
p fifo.running
p r3_writer.active
p r3_writer.uncertain
p r3_writer.needs_power_cycle
p r3_card
if fifo.running || r3_writer.active || r3_writer.uncertain || r3_writer.needs_power_cycle
 quit 12
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\initial-flash.bin 0x08000000 0x08200000

detach
quit
