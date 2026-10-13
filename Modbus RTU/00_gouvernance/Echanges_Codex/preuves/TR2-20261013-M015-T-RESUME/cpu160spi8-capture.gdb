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
if bench_trace_count > 1024 || bench_payload_count > 65536 || bench_preclose_count > 2048 || bench_result.count > 4736 || official_text_count > 2048
 quit 6
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-T-resume-20261013T084635Z/cpu160spi8-result.bin &bench_result (&bench_result+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-T-resume-20261013T084635Z/cpu160spi8-traces.bin bench_trace ((char *)bench_trace)+bench_trace_count*sizeof(BenchTrace)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-T-resume-20261013T084635Z/cpu160spi8-payload.bin bench_payload bench_payload+bench_payload_count
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-T-resume-20261013T084635Z/cpu160spi8-preclose.bin bench_preclose ((char *)bench_preclose)+bench_preclose_count*sizeof(BenchPreclose)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-T-resume-20261013T084635Z/cpu160spi8-overflow.bin &bench_preclose_overflow (&bench_preclose_overflow+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-T-resume-20261013T084635Z/cpu160spi8-log-refused.bin &bench_log_refused (&bench_log_refused+1)
if bench_result.count > 0
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-T-resume-20261013T084635Z/cpu160spi8-official-words.bin official_words official_words+bench_result.count*7
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-T-resume-20261013T084635Z/cpu160spi8-ctrl10.bin &official_ctrl10 (&official_ctrl10+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-T-resume-20261013T084635Z/cpu160spi8-window.bin &fifo_window (&fifo_window+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-T-resume-20261013T084635Z/cpu160spi8-RCC.bin 0x46020C00 0x46020CF0
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-T-resume-20261013T084635Z/cpu160spi8-PWR.bin 0x46020800 0x46020820
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-T-resume-20261013T084635Z/cpu160spi8-FLASH-ACR.bin 0x40022000 0x40022004

detach
quit
