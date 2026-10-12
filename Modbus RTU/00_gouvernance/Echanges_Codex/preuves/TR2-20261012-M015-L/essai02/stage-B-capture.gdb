set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
p bench_result
p bench_trace_count
p bench_payload_count
p sizeof(BenchResult)
p sizeof(BenchTrace)
p bench_uid
p bench_core_hz
x/2wx 0xE000ED28
x/6wx 0x42020800
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-L-attempt02-20261012T201926Z/stage-B-result.bin &bench_result (&bench_result+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-L-attempt02-20261012T201926Z/stage-B-traces.bin bench_trace ((char *)bench_trace)+bench_trace_count*sizeof(BenchTrace)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-L-attempt02-20261012T201926Z/stage-B-payload.bin bench_payload bench_payload+bench_payload_count
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-L-attempt02-20261012T201926Z/stage-B-trace-count.bin &bench_trace_count (&bench_trace_count+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-L-attempt02-20261012T201926Z/stage-B-ram.bin 0x20000000 0x200C0000

detach
quit
