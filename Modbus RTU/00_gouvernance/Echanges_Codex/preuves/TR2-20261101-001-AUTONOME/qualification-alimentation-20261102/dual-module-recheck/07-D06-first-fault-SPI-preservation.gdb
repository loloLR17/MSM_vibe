set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result
p bench_trace_count
p bench_payload_count
p bench_preclose_count
p bench_preclose_overflow
p bench_log_refused
p r3_last_clock_bracket
p r3_clock_fault
p r3_card.commands
p r3_card.reads
p r3_card.writes
p r3_card.initialized
p r3_card.write_enabled
p r3_retained.payload.sensor_only
p fifo.running
p fifo.stopped
p r3_writer.active
p/x *(unsigned int *)0x42021414
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\dual-module-recheck\D06-spi-fault\trace.bin &bench_trace (char *)&bench_trace+sizeof(bench_trace)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\dual-module-recheck\D06-spi-fault\payload.bin &bench_payload (char *)&bench_payload+sizeof(bench_payload)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\dual-module-recheck\D06-spi-fault\command-rx.bin &bench_command_rx (char *)&bench_command_rx+sizeof(bench_command_rx)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\dual-module-recheck\D06-spi-fault\preclose.bin &bench_preclose (char *)&bench_preclose+sizeof(bench_preclose)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\dual-module-recheck\D06-spi-fault\mapper.bin &mapper (char *)&mapper+sizeof(mapper)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\dual-module-recheck\D06-spi-fault\last-clock-bracket.bin &r3_last_clock_bracket (char *)&r3_last_clock_bracket+sizeof(r3_last_clock_bracket)

detach
quit
