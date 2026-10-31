set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61236
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result.stage
p r3_result.run
p r3_result.error
if r3_result.stage!=2 || r3_result.run || r3_result.error || r3_clock_fault || r3_command || fifo.running || r3_writer.active
 quit 8
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-r3-N8D-reference-20261031/hardware/reference-image.bin 0x08000000 0x800cf28

detach
quit
