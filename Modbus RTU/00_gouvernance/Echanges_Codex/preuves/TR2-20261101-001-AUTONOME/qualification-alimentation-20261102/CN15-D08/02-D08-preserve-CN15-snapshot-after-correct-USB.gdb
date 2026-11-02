set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\CN15-D08\terminal\snapshot.bin &r3_retained (char *)&r3_retained+sizeof(r3_retained)
set print elements 64
info registers pc
p/x *(unsigned int *)0x46020cf4
p r3_result
p standalone_state
p r3_retained_available
p/x r3_retained.guard
p r3_retained.payload.result
p r3_retained.payload.mapper
p r3_retained.payload.writer
p r3_retained.payload.clock_fault
p r3_retained.payload.sensor_only
p r3_retained.payload.budget_mask
p r3_retained.payload.maximum_cycles
p r3_card.commands
p r3_card.reads
p r3_card.writes
p fifo.running
p fifo.stopped
p r3_writer.active
p/x *(unsigned int *)0x42021414
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\CN15-D08\terminal\result.bin &r3_retained.payload.result (char *)&r3_retained.payload.result+sizeof(r3_retained.payload.result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\CN15-D08\terminal\source-sha.bin &r3_retained.payload.source_sha256 (char *)&r3_retained.payload.source_sha256+sizeof(r3_retained.payload.source_sha256)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\CN15-D08\flash-readback.bin 0x08000000 0x800c6e0

detach
quit
