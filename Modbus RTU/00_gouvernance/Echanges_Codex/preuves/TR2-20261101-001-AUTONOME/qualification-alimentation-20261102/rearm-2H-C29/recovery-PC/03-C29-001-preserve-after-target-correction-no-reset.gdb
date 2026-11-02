set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
set print elements 32
info registers pc sp lr xpsr
p/x *(unsigned int *)0x46020cf4
p r3_result
p standalone_state
p r3_retained_available
p/x r3_retained.guard
p fifo.running
p r3_writer.active
if r3_result.stage==3 && fifo.running
 echo ACTIVE001_AT_ATTACHMENT_NO_HEAVY_DUMP\n
 detach
 quit 17
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\rearm-2H-C29\reported-red-recovery\snapshot-first.bin &r3_retained (char *)&r3_retained+sizeof(r3_retained)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\rearm-2H-C29\reported-red-recovery\entire-SRAM.bin 0x20000000 0x200c0000
p r3_retained.payload.source_sha
p r3_retained.payload.result
p r3_retained.payload.writer
p r3_retained.payload.mapper
p r3_retained.payload.last_clock_bracket
p r3_retained.payload.queue.head
p r3_retained.payload.queue.tail
p r3_retained.payload.queue.count
p r3_retained.payload.frame_count
p r3_retained.payload.frame_seq
p r3_retained.payload.first_output
p r3_card
p r3_writer
p durable_status
p fifo.stopped
p/x *(unsigned int *)0x42021414
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\rearm-2H-C29\reported-red-recovery\live-result.bin &r3_result (char *)&r3_result+sizeof(r3_result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\rearm-2H-C29\reported-red-recovery\result.bin &r3_retained.payload.result (char *)&r3_retained.payload.result+sizeof(r3_retained.payload.result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\rearm-2H-C29\reported-red-recovery\queue.bin &r3_retained.payload.queue (char *)&r3_retained.payload.queue+sizeof(r3_retained.payload.queue)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\rearm-2H-C29\reported-red-recovery\source-sha.bin &r3_retained.payload.source_sha256 (char *)&r3_retained.payload.source_sha256+sizeof(r3_retained.payload.source_sha256)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\rearm-2H-C29\reported-red-recovery\header.bin &r3_retained.payload.header (char *)&r3_retained.payload.header+sizeof(r3_retained.payload.header)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\rearm-2H-C29\reported-red-recovery\partial-frame.bin &r3_retained.payload.partial_frame (char *)&r3_retained.payload.partial_frame+sizeof(r3_retained.payload.partial_frame)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\rearm-2H-C29\reported-red-recovery\live-card.bin &r3_card (char *)&r3_card+sizeof(r3_card)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\rearm-2H-C29\reported-red-recovery\image-readback.bin 0x08000000 0x800d05c

detach
quit
