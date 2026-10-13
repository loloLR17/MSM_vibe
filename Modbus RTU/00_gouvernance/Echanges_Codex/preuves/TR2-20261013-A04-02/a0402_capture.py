import sys,time
sys.path.insert(0,'/tmp');import a0402_hardware as h;import a0402_hp as hp
elf=h.P/'build-artifacts/tr2-cumulative-m014-m015-spi4.elf'
h.debug('capture-arm-gate',elf,h.IDENTITY+'''p cooperative.state
p cooperative_modbus_count
p tr2_irq_observer
if tr2_production_stage != 2 || tr2_production_result != 0 || cooperative.state != 1 || bench_ready != 1 || cooperative_arm || cooperative_start_tick || cooperative_modbus_count != 1
 quit 4
end
if g_serial.tx_busy || g_serial.tx_fault || g_serial.uart.ErrorCode || g_serial.event_head != g_serial.event_tail || tr2_irq_observer.events_dropped || tr2_irq_observer.errors_polled
 quit 5
end
set var bench_command=0x4D313556
''')
hp.invoke('Capture',hp.gate())
