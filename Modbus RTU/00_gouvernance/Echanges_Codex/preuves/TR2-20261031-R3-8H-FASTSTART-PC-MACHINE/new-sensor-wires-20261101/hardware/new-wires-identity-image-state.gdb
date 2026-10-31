set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61236
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result
p r3_command
p r3_clock_fault
p/x *(unsigned int*)0x46020cf4
if fifo.running || r3_writer.active || r3_command || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-r3-N8A-new-wires-20261101/hardware/N8A-readback.bin 0x08000000 0x800d51c

detach
quit
