from hardware import *
elf=P.parent/'probe-build-03/tr2-r4u-live-runtime-NON-PRODUCTION.elf'
body=IDENTITY+'''p r4u_result
p owner.acquisition.fifo.error
p owner.acquisition.fifo.stop_error
p owner.acquisition.fifo.running
p owner.acquisition.fifo.stopped
p bench_trace_count
p bench_payload_count
p bench_preclose_count
p bench_log_refused
p bench_preclose_overflow
p bench_trace[0]@bench_trace_count
p bench_preclose[0]@bench_preclose_count
p owner.acquisition.fifo.parser
p *(SPI_TypeDef*)0x40003C00
p g_serial.tx_busy
p g_serial.tx_fault
if owner.acquisition.fifo.running || !owner.acquisition.fifo.stopped || r4_result.write_bytes
 quit 4
end
'''+f'''dump binary memory {win(P/'ro04-failed-words.bin')} owner.acquisition.fifo.words owner.acquisition.fifo.words+896
dump binary memory {win(P/'ro04-spi-payload.bin')} bench_payload bench_payload+bench_payload_count
dump binary memory {win(P/'ro04-spi-trace.bin')} bench_trace bench_trace+bench_trace_count
dump binary memory {win(P/'ro04-spi-preclose.bin')} bench_preclose bench_preclose+bench_preclose_count
'''
debug('ro04-failure-observe',elf,body,40)
assert (P/'ro04-fram.bin').read_bytes()==(P/'fram-before.bin').read_bytes()
