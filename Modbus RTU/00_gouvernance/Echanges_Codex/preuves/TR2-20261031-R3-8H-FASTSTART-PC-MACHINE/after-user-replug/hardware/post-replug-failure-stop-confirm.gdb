set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61236
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p fifo.parser
p r3_result.stage
p r3_result.error
p fifo.running
p r3_writer.active
p r3_card.failed
p r3_clock_fault
p/x *(unsigned int*)0x42021414
if fifo.running || r3_writer.active || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-r3-8h-faststart-N8C-replug-20261031/n8c-card-calibrate/failing-fifo-words.bin fifo.words (fifo.words+64*7)

detach
quit
