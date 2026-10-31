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
if fifo.running || r3_writer.active
 quit 8
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-r3-8h-faststart-N8B-20261031/n8b-failed-gate/failing-fifo-words.bin fifo.words (fifo.words+64*7)

detach
quit
