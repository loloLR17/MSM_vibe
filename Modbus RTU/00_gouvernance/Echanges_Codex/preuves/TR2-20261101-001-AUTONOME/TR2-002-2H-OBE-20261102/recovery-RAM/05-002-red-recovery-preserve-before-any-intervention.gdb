set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
set print elements 32
info registers pc sp lr xpsr
p/x *(unsigned int *)0x46020cf4
p r3_result
p fifo.running
p fifo.stopped
p r3_writer.active
p r3_writer.uncertain
p r3_writer.needs_power_cycle
if r3_result.stage==3 && fifo.running
 echo ACTIVE_AT_ATTACHMENT_NO_HEAVY_DUMP\n
 detach
 quit 17
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\entire-SRAM.bin 0x20000000 0x200c0000
p fifo
p r3_writer
p r3_card
p durable_status
p r3_clock_fault
p q.head
p q.tail
p q.count
p q.borrowed
p durable_frame_count
p durable_frame_seq
p durable_first_output
p durable_frame_flags
p frame_first_time
p frame_last_time
p/x *(unsigned int *)0x42021414
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\result.bin &r3_result (char *)&r3_result+sizeof(r3_result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\fifo.bin &fifo (char *)&fifo+sizeof(fifo)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\card.bin &r3_card (char *)&r3_card+sizeof(r3_card)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\writer.bin &r3_writer (char *)&r3_writer+sizeof(r3_writer)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\queue.bin &q (char *)&q+sizeof(q)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\source-sha.bin &r3_source_sha (char *)&r3_source_sha+sizeof(r3_source_sha)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\source-hash-state.bin &source_hash (char *)&source_hash+sizeof(source_hash)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\partial-frame.bin &frame (char *)&frame+sizeof(frame)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\header.bin &r3_header (char *)&r3_header+sizeof(r3_header)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\live-footer.bin &durable_footer (char *)&durable_footer+sizeof(durable_footer)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\status.bin &durable_status (char *)&durable_status+sizeof(durable_status)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\raw.bin &r3_raw (char *)&r3_raw+sizeof(r3_raw)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\trace.bin &bench_trace (char *)&bench_trace+sizeof(bench_trace)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\payload.bin &bench_payload (char *)&bench_payload+sizeof(bench_payload)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\preclose.bin &bench_preclose (char *)&bench_preclose+sizeof(bench_preclose)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\distributions.bin &r3_distributions (char *)&r3_distributions+sizeof(r3_distributions)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\SD-records.bin &r3_sd_records (char *)&r3_sd_records+sizeof(r3_sd_records)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\reported-red-recovery\image-readback.bin 0x08000000 0x800cf28

detach
quit
