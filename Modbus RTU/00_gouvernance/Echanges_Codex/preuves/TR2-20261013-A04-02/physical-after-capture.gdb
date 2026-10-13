set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p SystemCoreClock
p cooperative
p bench_result
p cooperative_burst_count
p cooperative_modbus_count
p cooperative_modbus
p cooperative_prefix_xyz
p cooperative_prefix_tick
p cooperative_start_tick
p cooperative_end_tick
p cooperative_max_cycles
p cooperative_service_max_gap_cycles
p cooperative_modbus_max_cycles
p cooperative_modbus_capture_max_cycles
p cooperative_command_io_denied
p cooperative_command_snapshot_failures
p cooperative_fram_crc_before
p cooperative_fram_crc_after
p cooperative_fram_state
p tr2_fifo_state
p tr2_irq_observer
p tr2_production_poll_errors
p production_runtime.recovery
p g_serial.uart.ErrorCode
p g_serial.tx_fault
p g_serial.tx_busy
p bench_trace_count
p bench_payload_count
p bench_preclose_count
p bench_log_refused
p bench_preclose_overflow
x/3wx 0xE000E100
x/2wx 0xE000ED28
if cooperative.window.received > 36864 || cooperative_burst_count > 320 || cooperative_modbus_count > 8 || bench_trace_count > 1024 || bench_payload_count > 65536 || bench_preclose_count > 2048
 quit 6
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-engine.bin &cooperative (&cooperative+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-result.bin &bench_result (&bench_result+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-words.bin official_words official_words+cooperative.window.received*7
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-bursts.bin cooperative_bursts ((char *)cooperative_bursts)+cooperative_burst_count*sizeof(BurstEvidence)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-modbus.bin cooperative_modbus ((char *)cooperative_modbus)+cooperative_modbus_count*sizeof(ModbusEvidence)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-traces.bin bench_trace ((char *)bench_trace)+bench_trace_count*sizeof(BenchTrace)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-payload.bin bench_payload bench_payload+bench_payload_count
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-preclose.bin bench_preclose ((char *)bench_preclose)+bench_preclose_count*sizeof(BenchPreclose)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-persistent.bin production_runtime.candidate production_runtime.candidate+51914
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-cooperative_start_tick.bin &cooperative_start_tick (&cooperative_start_tick+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-cooperative_end_tick.bin &cooperative_end_tick (&cooperative_end_tick+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-cooperative_max_cycles.bin &cooperative_max_cycles (&cooperative_max_cycles+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-cooperative_service_max_gap_cycles.bin &cooperative_service_max_gap_cycles (&cooperative_service_max_gap_cycles+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-cooperative_modbus_max_cycles.bin &cooperative_modbus_max_cycles (&cooperative_modbus_max_cycles+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-cooperative_modbus_capture_max_cycles.bin &cooperative_modbus_capture_max_cycles (&cooperative_modbus_capture_max_cycles+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-cooperative_command_io_denied.bin &cooperative_command_io_denied (&cooperative_command_io_denied+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-cooperative_command_snapshot_failures.bin &cooperative_command_snapshot_failures (&cooperative_command_snapshot_failures+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-cooperative_fram_crc_before.bin &cooperative_fram_crc_before (&cooperative_fram_crc_before+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-cooperative_fram_crc_after.bin &cooperative_fram_crc_after (&cooperative_fram_crc_after+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-cooperative_fram_state.bin &cooperative_fram_state (&cooperative_fram_state+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-tr2_fifo_state.bin &tr2_fifo_state (&tr2_fifo_state+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-cooperative_log_first_word.bin &cooperative_log_first_word (&cooperative_log_first_word+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-SystemCoreClock.bin &SystemCoreClock (&SystemCoreClock+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-tr2_production_poll_errors.bin &tr2_production_poll_errors (&tr2_production_poll_errors+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-bench_log_refused.bin &bench_log_refused (&bench_log_refused+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/physical-bench_preclose_overflow.bin &bench_preclose_overflow (&bench_preclose_overflow+1)
if cooperative.state != 4 || cooperative.result || cooperative.cleanup_result || cooperative.window.xyz != 32768 || cooperative.window.timestamps != 4096 || cooperative_fram_state != 3 || cooperative_fram_crc_before != cooperative_fram_crc_after || production_runtime.recovery.generation != 12
 quit 7
end
if cooperative_command_io_denied != 1 || cooperative_command_snapshot_failures || tr2_production_poll_errors || tr2_irq_observer.events_dropped || tr2_irq_observer.errors_polled || g_serial.uart.ErrorCode || g_serial.tx_fault || g_serial.tx_busy || tr2_irq_observer.tx_started != 6 || tr2_irq_observer.tx_callback != 6
 quit 8
end

detach
quit
