from hardware import *
elf=P.parent/'probe-build-01/tr2-r4u-live-runtime-NON-PRODUCTION.elf'
debug('after-compose-refusal',elf,IDENTITY+'''p r4u_result
p r4_result
p work
p owner.acquisition.fifo.running
p owner.acquisition.metrology
p owner.acquisition.engine
p engine.has_active
p g_serial.uart.Instance
if owner.acquisition.fifo.running || r4_result.write_bytes || engine.has_active
 quit 4
end
set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
'''+f'dump binary memory {win(P/"refusal-fram.bin")} baseline baseline+262144\n',60)
assert (P/'refusal-fram.bin').read_bytes()==(P/'fram-before.bin').read_bytes()
