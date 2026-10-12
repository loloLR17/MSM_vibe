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
p official_packets
p official_ctrl10
p official_text_count
x/2wx 0xE000ED28
x/6wx 0x42020800
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-P-20261012T221333Z/control-result.bin &bench_result (&bench_result+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-P-20261012T221333Z/control-traces.bin bench_trace ((char *)bench_trace)+bench_trace_count*sizeof(BenchTrace)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-P-20261012T221333Z/control-payload.bin bench_payload bench_payload+bench_payload_count
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-P-20261012T221333Z/control-preclose.bin bench_preclose ((char *)bench_preclose)+bench_preclose_count*sizeof(BenchPreclose)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-P-20261012T221333Z/control-overflow.bin &bench_preclose_overflow (&bench_preclose_overflow+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-P-20261012T221333Z/control-log-refused.bin &bench_log_refused (&bench_log_refused+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-P-20261012T221333Z/control-poll-aggregate.bin bench_poll_aggregate bench_poll_aggregate+6
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-P-20261012T221333Z/control-official-words.bin official_words official_words+bench_result.count*7
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-P-20261012T221333Z/control-official-text.bin official_text official_text+official_text_count
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-P-20261012T221333Z/control-ctrl10.bin &official_ctrl10 (&official_ctrl10+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-P-20261012T221333Z/control-method.bin &bench_transfer_method (&bench_transfer_method+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-P-20261012T221333Z/control-command-rx.bin bench_command_rx bench_command_rx+bench_trace_count

detach
quit
