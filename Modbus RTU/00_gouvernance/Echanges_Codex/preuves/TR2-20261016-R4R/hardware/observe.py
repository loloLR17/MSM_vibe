from hardware import *
elf=P.parent/'probe-build/tr2-r4r-physical-time-NON-PRODUCTION.elf'
debug('r4r-diagnostic-result',elf,IDENTITY+'''p r4r_result
p r4_result
p work
p owner.acquisition.fifo
p engine.status
p store.epoch
p store.effect
p/x *(unsigned int*)0xE000E100
p/x *(unsigned int*)0xE000E104
'''+f'''dump binary memory {win(P/'raw.bin')} r4r_raw r4r_raw+7*r4r_result.raw_words
dump binary memory {win(P/'brackets.bin')} r4r_brackets &r4r_brackets[r4r_result.brackets]
set $read=(int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $read
if $read != 0
 quit 7
end
dump binary memory {win(P/'fram-after-probe.bin')} baseline baseline+262144
''',timeout=60)
a=(P/'fram-after-probe.bin').read_bytes();assert a==(P/'fram-before.bin').read_bytes()
print('Full FRAM unchanged; diagnostic log preserved',flush=True)
