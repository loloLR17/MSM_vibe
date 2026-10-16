from hardware import *
elf=P.parent/'probe-build-02/tr2-r4s-readonly-service-NON-PRODUCTION.elf'
debug('phase1b-terminal-fram',elf,IDENTITY+'''p owner.acquisition.fifo.stopped
p r4s_result
if owner.acquisition.fifo.running || !owner.acquisition.fifo.stopped
 quit 5
end
set $r=(int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r
if $r != 0
 quit 6
end
'''+f'dump binary memory {win(P/"fram-after-phase1b.bin")} baseline baseline+262144\n')
assert (P/'fram-after-phase1b.bin').read_bytes()==(P/'fram-before.bin').read_bytes()
print('Full FRAM exact; result already acquired, no test repeated')
