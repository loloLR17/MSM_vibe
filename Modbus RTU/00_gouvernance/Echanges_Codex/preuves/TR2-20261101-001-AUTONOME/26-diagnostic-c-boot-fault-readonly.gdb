set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result
p standalone_state
p durable_status
p r3_card
p r3_writer
p fifo
p mapper
p r3_clock_fault
p r3_spi_fault
p r3_distributions[0].max
p r3_distributions[1].max
p r3_distributions[2].max
p q.count
p q.tail
p q.head
p q.borrowed
p durable_frame_count
p durable_frame_seq
p bench_trace_count
p bench_payload_count
p/x r3_source_sha
p sizeof(source_hash)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\diagnostic-a1c-results\result.bin &r3_result ((char *)&r3_result)+sizeof(r3_result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\diagnostic-a1c-results\status.bin &durable_status ((char *)&durable_status)+sizeof(durable_status)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\diagnostic-a1c-results\source-sha.bin &r3_source_sha ((char *)&r3_source_sha)+sizeof(r3_source_sha)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\diagnostic-a1c-results\source-hash-state.bin &source_hash ((char *)&source_hash)+sizeof(source_hash)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\diagnostic-a1c-results\header.bin &r3_header ((char *)&r3_header)+sizeof(r3_header)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\diagnostic-a1c-results\footer.bin &durable_footer ((char *)&durable_footer)+sizeof(durable_footer)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\diagnostic-a1c-results\raw-prefix.bin &r3_raw ((char *)&r3_raw)+sizeof(r3_raw)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\diagnostic-a1c-results\trace.bin &bench_trace ((char *)&bench_trace)+sizeof(bench_trace)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\diagnostic-a1c-results\payload.bin &bench_payload ((char *)&bench_payload)+sizeof(bench_payload)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\diagnostic-a1c-results\queue.bin &q ((char *)&q)+sizeof(q)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\diagnostic-a1c-results\partial-frame.bin &frame ((char *)&frame)+sizeof(frame)

detach
quit
