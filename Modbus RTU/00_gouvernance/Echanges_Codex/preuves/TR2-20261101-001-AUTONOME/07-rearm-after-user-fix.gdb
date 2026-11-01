set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result
if r3_result.stage != 99 || r3_result.error != 2 || r3_card.writes || r3_card.write_enabled || r3_writer.active
 quit 8
end
monitor reset

detach
quit
