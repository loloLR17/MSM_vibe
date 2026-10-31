set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61236
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result.stage
p r3_result.error
p fifo.parser
p fifo.running
p fifo.stop_error
p r3_writer.active
p r3_clock_fault
p/x *(unsigned int*)0x42021414
p/x *(unsigned int*)0x46020cf4
x/2wx 0x46002008
x/2wx 0x40003808
if fifo.running || fifo.stop_error || r3_writer.active || r3_clock_fault || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-r3-N8A-restored-20261031/restored-N8A-calibration/failing-fifo-words.bin fifo.words (fifo.words+64*7)

detach
quit
