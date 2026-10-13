set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
p tr2_fifo_state
p tr2_fifo_fault
p tr2_fifo_fram_status
p bench_result
p fifo_window
p bench_uid
p official_ctrl10
x/2wx 0xE000ED28
if bench_trace_count > 1024 || bench_payload_count > 65536 || bench_preclose_count > 2048 || bench_result.count > 4736
 quit 6
end
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/qualification/primary-result.bin &bench_result (&bench_result+1)
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/qualification/primary-traces.bin bench_trace ((char *)bench_trace)+bench_trace_count*sizeof(BenchTrace)
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/qualification/primary-payload.bin bench_payload bench_payload+bench_payload_count
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/qualification/primary-preclose.bin bench_preclose ((char *)bench_preclose)+bench_preclose_count*sizeof(BenchPreclose)
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/qualification/primary-overflow.bin &bench_preclose_overflow (&bench_preclose_overflow+1)
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/qualification/primary-log-refused.bin &bench_log_refused (&bench_log_refused+1)
if bench_result.count > 0
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/qualification/primary-official-words.bin official_words official_words+bench_result.count*7
end
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/qualification/primary-ctrl10.bin &official_ctrl10 (&official_ctrl10+1)
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/qualification/primary-window.bin &fifo_window (&fifo_window+1)
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/qualification/primary-RCC.bin 0x46020C00 0x46020CF0
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/qualification/primary-PWR.bin 0x46020800 0x46020820
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/qualification/primary-FLASH-ACR.bin 0x40022000 0x40022004

detach
quit
