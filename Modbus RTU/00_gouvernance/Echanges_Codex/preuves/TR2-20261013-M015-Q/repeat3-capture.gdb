set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
p bench_result
p bench_trace_count
p bench_payload_count
p bench_preclose_count
p bench_uid
p bench_core_hz
p official_ctrl10
x/2wx 0xE000ED28
if bench_trace_count > 1024 || bench_payload_count > 65536 || bench_preclose_count > 2048 || bench_result.count > 1024 || official_text_count > 2048
 quit 6
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-Q-20261012T224111Z/repeat3-result.bin &bench_result (&bench_result+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-Q-20261012T224111Z/repeat3-traces.bin bench_trace ((char *)bench_trace)+bench_trace_count*sizeof(BenchTrace)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-Q-20261012T224111Z/repeat3-payload.bin bench_payload bench_payload+bench_payload_count
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-Q-20261012T224111Z/repeat3-preclose.bin bench_preclose ((char *)bench_preclose)+bench_preclose_count*sizeof(BenchPreclose)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-Q-20261012T224111Z/repeat3-overflow.bin &bench_preclose_overflow (&bench_preclose_overflow+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-Q-20261012T224111Z/repeat3-log-refused.bin &bench_log_refused (&bench_log_refused+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-Q-20261012T224111Z/repeat3-official-words.bin official_words official_words+bench_result.count*7
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-Q-20261012T224111Z/repeat3-ctrl10.bin &official_ctrl10 (&official_ctrl10+1)

detach
quit
