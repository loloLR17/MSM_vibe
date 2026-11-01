set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result
p r3_writer.active
p fifo.running
p r3_card.writes
if r3_writer.active || fifo.running || r3_card.writes
 quit 9
end
p sizeof(r3_retained)
p sizeof(r3_retained.payload)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\retained-a1f-reset-test\flash.bin 0x08000000 0x800cecc
restore \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\retained-a1f-reset-test\synthetic-fixture.bin binary &r3_retained
p/x r3_retained.guard
monitor reset

detach
quit
