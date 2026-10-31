set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61236
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result
p r3_clock_fault
p/x *(unsigned int*)0x46020cf4
x/3wx 0x46020c1c
x/3wx 0x46020ce0
x/6wx 0x42020800
x/2wx 0x42020820
if fifo.running || r3_writer.active || r3_command || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-r3-N8A-restored-20261031/hardware/previous-D-before-restore.bin 0x08000000 0x800cf28

detach
quit
