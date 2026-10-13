set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p cooperative
p tr2_fifo_state
p cooperative_fram_state
p cooperative_modbus_count
p cooperative_modbus
p cooperative_burst_count
p cooperative_prefix_xyz
p cooperative_prefix_tick
p cooperative_max_cycles
p cooperative_fram_crc_before
p cooperative_fram_crc_after
p bench_trace_count
p bench_payload_count
p bench_preclose_count
p bench_log_refused
p bench_preclose_overflow
p g_serial
p production_runtime.recovery
p tr2_irq_observer
p tr2_production_poll_errors
p g_serial.uart.ErrorCode
p g_serial.tx_fault
x/2wx 0xE000ED28
if bench_trace_count > 1024 || bench_payload_count > 65536 || bench_preclose_count > 2048 || cooperative.window.received > 36864 || cooperative_burst_count > 320
 quit 6
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/coop4-engine.bin &cooperative (&cooperative+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/coop4-result.bin &bench_result (&bench_result+1)
if bench_trace_count > 0
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/coop4-traces.bin bench_trace ((char *)bench_trace)+bench_trace_count*sizeof(BenchTrace)
end
if bench_payload_count > 0
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/coop4-payload.bin bench_payload bench_payload+bench_payload_count
end
if bench_preclose_count > 0
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/coop4-preclose.bin bench_preclose ((char *)bench_preclose)+bench_preclose_count*sizeof(BenchPreclose)
end
if cooperative.window.received > 0
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/coop4-words.bin official_words official_words+cooperative.window.received*7
end
if cooperative_burst_count > 0
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/coop4-bursts.bin cooperative_bursts ((char *)cooperative_bursts)+cooperative_burst_count*sizeof(BurstEvidence)
end
if cooperative_modbus_count > 0
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/coop4-modbus.bin cooperative_modbus ((char *)cooperative_modbus)+cooperative_modbus_count*sizeof(ModbusEvidence)
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/coop4-log-first.bin &cooperative_log_first_word (&cooperative_log_first_word+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/coop4-start.bin &cooperative_start_tick (&cooperative_start_tick+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/coop4-end.bin &cooperative_end_tick (&cooperative_end_tick+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/coop4-max-cycles.bin &cooperative_max_cycles (&cooperative_max_cycles+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/coop4-prefix-xyz.bin cooperative_prefix_xyz (cooperative_prefix_xyz+8)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/coop4-prefix-tick.bin cooperative_prefix_tick (cooperative_prefix_tick+8)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/persistent-after-capture.bin production_runtime.candidate (production_runtime.candidate+51914)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/coop4-RCC.bin 0x46020C00 0x46020CF0

detach
quit
