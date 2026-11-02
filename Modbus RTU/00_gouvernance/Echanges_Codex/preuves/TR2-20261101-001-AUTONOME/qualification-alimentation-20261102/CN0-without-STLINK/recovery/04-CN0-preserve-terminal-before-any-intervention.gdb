set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
set print elements 32
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\CN0-without-STLINK\terminal-recovery\snapshot.bin &r3_retained (char *)&r3_retained+sizeof(r3_retained)
info registers pc
p r3_result
p standalone_state
p r3_retained_available
p/x r3_retained.guard
p r3_retained.payload.source_sha
echo RETAINED_RESULT_BEGIN

p r3_retained.payload.result
echo RETAINED_RESULT_END

p r3_retained.payload.writer
p r3_retained.payload.mapper
p r3_retained.payload.last_clock_bracket
p r3_retained.payload.budget_mask
p r3_retained.payload.maximum_cycles
p r3_retained.payload.queue.head
p r3_retained.payload.queue.tail
p r3_retained.payload.queue.count
p r3_retained.payload.frame_count
p r3_retained.payload.frame_seq
p r3_retained.payload.clock_fault
p fifo.running
p r3_writer.active
p r3_writer.uncertain
p r3_writer.needs_power_cycle
p r3_card.writes
p/x *(unsigned int *)0x42021414
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\CN0-without-STLINK\terminal-recovery\result.bin &r3_retained.payload.result (char *)&r3_retained.payload.result+sizeof(r3_retained.payload.result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\CN0-without-STLINK\terminal-recovery\writer.bin &r3_retained.payload.writer (char *)&r3_retained.payload.writer+sizeof(r3_retained.payload.writer)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\CN0-without-STLINK\terminal-recovery\queue.bin &r3_retained.payload.queue (char *)&r3_retained.payload.queue+sizeof(r3_retained.payload.queue)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\CN0-without-STLINK\terminal-recovery\producer-sha.bin &r3_retained.payload.source_sha256 (char *)&r3_retained.payload.source_sha256+sizeof(r3_retained.payload.source_sha256)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\CN0-without-STLINK\terminal-recovery\header.bin &r3_retained.payload.header (char *)&r3_retained.payload.header+sizeof(r3_retained.payload.header)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\CN0-without-STLINK\terminal-recovery\partial-frame.bin &r3_retained.payload.partial_frame (char *)&r3_retained.payload.partial_frame+sizeof(r3_retained.payload.partial_frame)

detach
quit
