set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if r3_result.stage != 4 || r3_result.error || fifo.running || r3_writer.active || r3_writer.uncertain || q.count
 quit 8
end
p r3_distributions[0].max
p r3_distributions[1].max
p r3_distributions[2].max
p r3_distributions[3].max
p r3_card.write_enabled
p r3_card.failed

detach
quit
