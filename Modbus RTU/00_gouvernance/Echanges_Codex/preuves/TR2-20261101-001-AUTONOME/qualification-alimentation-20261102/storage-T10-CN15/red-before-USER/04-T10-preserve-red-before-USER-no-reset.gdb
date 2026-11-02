set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\storage-T10-CN15\red-before-USER\snapshot.bin &r3_retained (char *)&r3_retained+sizeof(r3_retained)
set print elements 64
info registers pc
p/x *(unsigned int *)0x46020cf4
p r3_result
p standalone_state
p standalone_fs_readback
p r3_retained_available
p/x r3_retained.guard
p r3_retained.payload.source_sha
p r3_retained.payload.result
p durable_status
p fifo.running
p fifo.stopped
p r3_writer
p r3_card
p r3_clock_fault
p bench_trace_count
p bench_payload_count
p bench_preclose_count
p/x *(unsigned int *)0x42021414
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\storage-T10-CN15\red-before-USER\result.bin &r3_result (char *)&r3_result+sizeof(r3_result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\storage-T10-CN15\red-before-USER\status.bin &durable_status (char *)&durable_status+sizeof(durable_status)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\storage-T10-CN15\red-before-USER\card.bin &r3_card (char *)&r3_card+sizeof(r3_card)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\storage-T10-CN15\red-before-USER\brackets.bin &r3_brackets (char *)&r3_brackets+sizeof(r3_brackets)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\storage-T10-CN15\red-before-USER\trace.bin &bench_trace (char *)&bench_trace+sizeof(bench_trace)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\storage-T10-CN15\red-before-USER\payload.bin &bench_payload (char *)&bench_payload+sizeof(bench_payload)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\storage-T10-CN15\red-before-USER\flash.bin 0x08000000 0x800d05c

detach
quit
