set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if r3_result.error || r3_result.stage!=2 || r3_result.run || fifo.running || r3_writer.active
 quit 12
end
thbreak r3_finished
set var r3_command=0x44335201
continue
delete breakpoints
p r3_result
p r3_card
p durable_status
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\initialized\result.bin &r3_result (char *)&r3_result+sizeof(r3_result)
if r3_result.error || r3_result.stage!=4 || r3_result.run!=1 || fifo.running || r3_writer.active || r3_result.who!=0x7b || r3_card.sectors!=62333952
 quit 13
end
set $pre3=(int)durable_preflight(3,131072)
p $pre3
if $pre3
 quit 14
end
set $pre5=(int)durable_preflight(5,671088640)
p $pre5
if $pre5
 quit 15
end
p durable_status

detach
quit
