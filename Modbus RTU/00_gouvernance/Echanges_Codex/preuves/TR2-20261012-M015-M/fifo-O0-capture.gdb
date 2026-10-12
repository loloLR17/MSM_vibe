set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
p bench_result
p bench_trace_count
p bench_payload_count
p bench_preclose_count
p bench_preclose_overflow
p sizeof(BenchResult)
p sizeof(BenchTrace)
p sizeof(BenchPreclose)
p bench_uid
p bench_core_hz
x/2wx 0xE000ED28
x/6wx 0x42020800
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-M-20261012T204419Z/fifo-O0-result.bin &bench_result (&bench_result+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-M-20261012T204419Z/fifo-O0-traces.bin bench_trace ((char *)bench_trace)+bench_trace_count*sizeof(BenchTrace)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-M-20261012T204419Z/fifo-O0-payload.bin bench_payload bench_payload+bench_payload_count
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-M-20261012T204419Z/fifo-O0-preclose.bin bench_preclose ((char *)bench_preclose)+bench_preclose_count*sizeof(BenchPreclose)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-M-20261012T204419Z/fifo-O0-overflow.bin &bench_preclose_overflow (&bench_preclose_overflow+1)

detach
quit
