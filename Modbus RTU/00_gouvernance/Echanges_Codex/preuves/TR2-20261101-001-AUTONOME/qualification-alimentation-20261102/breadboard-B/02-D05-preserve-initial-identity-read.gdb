set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p bench_trace_count
p bench_payload_count
p bench_preclose_count
p bench_trace[0]
x/8bx bench_payload
p fifo.initialized
p fifo.running
p r3_writer.active
p r3_card.commands
p r3_card.reads
p r3_card.writes
p r3_card.initialized
p r3_card.write_enabled
p/x *(unsigned int *)0x42021414
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\breadboard-B\D05-result.bin &r3_result (char *)&r3_result+sizeof(r3_result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\breadboard-B\D05-trace.bin &bench_trace (char *)&bench_trace+sizeof(bench_trace)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\breadboard-B\D05-payload.bin &bench_payload (char *)&bench_payload+sizeof(bench_payload)

detach
quit
