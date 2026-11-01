set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
set print elements 80
p r3_result
p standalone_state
p durable_status
p r3_writer
p fifo
p mapper
p r3_clock_fault
p r3_spi_fault
p r3_card
p q.head
p q.tail
p q.count
p q.borrowed
p durable_frame_count
p durable_frame_seq
p durable_first_output
p source_hash.bytes
p r3_distributions[0].max
p r3_distributions[1].max
p r3_distributions[2].max
p r3_distributions[3].max
p/x r3_source_sha
p/x *(unsigned int *)0x46020cf4
p/x *(unsigned int *)0x42021414
p/x *(unsigned int *)0x40003808
bt
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\sd2mhz-a1e-terminal\result.bin &r3_result ((char *)&r3_result)+sizeof(r3_result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\sd2mhz-a1e-terminal\status.bin &durable_status ((char *)&durable_status)+sizeof(durable_status)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\sd2mhz-a1e-terminal\writer.bin &r3_writer ((char *)&r3_writer)+sizeof(r3_writer)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\sd2mhz-a1e-terminal\source-sha.bin &r3_source_sha ((char *)&r3_source_sha)+sizeof(r3_source_sha)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\sd2mhz-a1e-terminal\source-hash-state.bin &source_hash ((char *)&source_hash)+sizeof(source_hash)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\sd2mhz-a1e-terminal\header.bin &r3_header ((char *)&r3_header)+sizeof(r3_header)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\sd2mhz-a1e-terminal\footer.bin &durable_footer ((char *)&durable_footer)+sizeof(durable_footer)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\sd2mhz-a1e-terminal\queue.bin &q ((char *)&q)+sizeof(q)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\sd2mhz-a1e-terminal\partial-frame.bin &frame ((char *)&frame)+sizeof(frame)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\sd2mhz-a1e-terminal\trace.bin &bench_trace ((char *)&bench_trace)+sizeof(bench_trace)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\sd2mhz-a1e-terminal\payload.bin &bench_payload ((char *)&bench_payload)+sizeof(bench_payload)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\sd2mhz-a1e-terminal\sd-records.bin &r3_sd_records ((char *)&r3_sd_records)+sizeof(r3_sd_records)

detach
quit
