from pathlib import Path
import json,sys
sys.path.insert(0,'/tmp');import a0402_hardware as h
hp=json.loads((h.P/'hp-capture-result.json').read_text());assert hp['passed'] and hp['serialClosed']
elf=h.P/'build-artifacts/tr2-cumulative-m014-m015-spi4.elf';prefix=h.win(h.P).replace('\\','/')
body=h.IDENTITY+'''p SystemCoreClock
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
'''
for label,start,end in [('engine','&cooperative','(&cooperative+1)'),('result','&bench_result','(&bench_result+1)'),('words','official_words','official_words+cooperative.window.received*7'),('bursts','cooperative_bursts','((char *)cooperative_bursts)+cooperative_burst_count*sizeof(BurstEvidence)'),('modbus','cooperative_modbus','((char *)cooperative_modbus)+cooperative_modbus_count*sizeof(ModbusEvidence)'),('traces','bench_trace','((char *)bench_trace)+bench_trace_count*sizeof(BenchTrace)'),('payload','bench_payload','bench_payload+bench_payload_count'),('preclose','bench_preclose','((char *)bench_preclose)+bench_preclose_count*sizeof(BenchPreclose)'),('persistent','production_runtime.candidate','production_runtime.candidate+51914')]:
 body+=f'dump binary memory {prefix}/physical-{label}.bin {start} {end}\n'
for name in ['cooperative_start_tick','cooperative_end_tick','cooperative_max_cycles','cooperative_service_max_gap_cycles','cooperative_modbus_max_cycles','cooperative_modbus_capture_max_cycles','cooperative_command_io_denied','cooperative_command_snapshot_failures','cooperative_fram_crc_before','cooperative_fram_crc_after','cooperative_fram_state','tr2_fifo_state','cooperative_log_first_word','SystemCoreClock','tr2_production_poll_errors','bench_log_refused','bench_preclose_overflow']:
 body+=f'dump binary memory {prefix}/physical-{name}.bin &{name} (&{name}+1)\n'
body+='''if cooperative.state != 4 || cooperative.result || cooperative.cleanup_result || cooperative.window.xyz != 32768 || cooperative.window.timestamps != 4096 || cooperative_fram_state != 3 || cooperative_fram_crc_before != cooperative_fram_crc_after || production_runtime.recovery.generation != 12
 quit 7
end
if cooperative_command_io_denied != 1 || cooperative_command_snapshot_failures || tr2_production_poll_errors || tr2_irq_observer.events_dropped || tr2_irq_observer.errors_polled || g_serial.uart.ErrorCode || g_serial.tx_fault || g_serial.tx_busy || tr2_irq_observer.tx_started != 6 || tr2_irq_observer.tx_callback != 6
 quit 8
end
'''
h.debug('physical-after-capture',elf,body)
assert (h.P/'physical-persistent.bin').read_bytes()==(h.P/'persistent-before.bin').read_bytes()
print('Physical acquisition/cleanup/FRAM/Modbus/UART gates all passed; no reset or restoration needed.')
