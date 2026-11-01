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
p r3_retained.payload.result
p r3_retained.payload.writer
p r3_retained.payload.queue.head
p r3_retained.payload.queue.tail
p r3_retained.payload.queue.count
p r3_retained.payload.queue.borrowed
p r3_retained.payload.frame_count
p r3_retained.payload.frame_seq
p r3_retained.payload.first_output
p r3_retained.payload.clock_fault
p r3_retained.payload.source_hash.bytes
p/x r3_retained.payload.source_sha256
p r3_result
p r3_clock_fault
p r3_card.writes
p r3_card.error
p r3_writer.active
p fifo.running
p/x *(unsigned int *)0x46020cf4
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\retained-a1f-real-result\snapshot.bin &r3_retained (char *)&r3_retained+sizeof(r3_retained)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\retained-a1f-real-result\result.bin &r3_retained.payload.result (char *)&r3_retained.payload.result+sizeof(r3_retained.payload.result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\retained-a1f-real-result\writer.bin &r3_retained.payload.writer (char *)&r3_retained.payload.writer+sizeof(r3_retained.payload.writer)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\retained-a1f-real-result\queue.bin &r3_retained.payload.queue (char *)&r3_retained.payload.queue+sizeof(r3_retained.payload.queue)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\retained-a1f-real-result\source-hash-state.bin &r3_retained.payload.source_hash (char *)&r3_retained.payload.source_hash+sizeof(r3_retained.payload.source_hash)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\retained-a1f-real-result\source-sha.bin &r3_retained.payload.source_sha256 (char *)&r3_retained.payload.source_sha256+sizeof(r3_retained.payload.source_sha256)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\retained-a1f-real-result\header.bin &r3_retained.payload.header (char *)&r3_retained.payload.header+sizeof(r3_retained.payload.header)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\retained-a1f-real-result\partial-frame.bin &r3_retained.payload.partial_frame (char *)&r3_retained.payload.partial_frame+sizeof(r3_retained.payload.partial_frame)

detach
quit
