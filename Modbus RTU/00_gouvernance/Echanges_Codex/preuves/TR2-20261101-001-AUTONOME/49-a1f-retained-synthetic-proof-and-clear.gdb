set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_retained_available
p/x r3_retained.guard
p r3_retained.payload.source_sha
p/x r3_retained.payload.result.error
p r3_result
p r3_card.writes
if r3_retained_available != 1 || r3_retained.payload.result.error != 0x52535431
 quit 10
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\retained-a1f-reset-test\retained-after-reset.bin &r3_retained (char *)&r3_retained+sizeof(r3_retained)
set r3_retained.guard.magic = 0
set r3_retained_available = 0
p r3_retained.guard.magic

detach
quit
