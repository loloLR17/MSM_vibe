set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p/x *(unsigned int *)0x46020cf4
p r3_irqs
p r3_result.stage
p r3_result.error
p r3_card.reads
p r3_card.writes
p standalone_state
p durable_status

detach
quit
