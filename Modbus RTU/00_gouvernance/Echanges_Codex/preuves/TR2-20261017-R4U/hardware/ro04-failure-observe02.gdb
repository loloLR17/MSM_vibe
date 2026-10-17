set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4u_result
p owner.acquisition.fifo.error
p owner.acquisition.fifo.stop_error
p owner.acquisition.fifo.running
p owner.acquisition.fifo.stopped
p bench_trace_count
p bench_payload_count
p bench_preclose_count
p bench_log_refused
p bench_preclose_overflow
p bench_trace[0]@bench_trace_count
p bench_preclose[0]@bench_preclose_count
p owner.acquisition.fifo.parser
p *hspi3.Instance
p g_serial.tx_busy
p g_serial.tx_fault
if owner.acquisition.fifo.running || !owner.acquisition.fifo.stopped || r4_result.write_bytes
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4u\hardware\ro04-failed-words.bin owner.acquisition.fifo.words owner.acquisition.fifo.words+896
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4u\hardware\ro04-spi-payload.bin bench_payload bench_payload+bench_payload_count
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4u\hardware\ro04-spi-trace.bin bench_trace bench_trace+bench_trace_count
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4u\hardware\ro04-spi-preclose.bin bench_preclose bench_preclose+bench_preclose_count

detach
quit
