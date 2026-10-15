from hardware import *
elf=P.parent/'bench-build/tr2-r4-qualification-NON-PRODUCTION.elf'
debug('bench3-readonly-final',elf,IDENTITY+'''p r4_result
p work
if r4_result.stage != 10 || r4_result.error != 0 || r4_result.protected_equal != 1 || r4_result.write_bytes != 0
 quit 6
end
monitor reset
''')
time.sleep(2)
debug('bench3-r2-trigger',elf,IDENTITY+'''p r4_result
if r4_result.stage != 1 || r4_result.error != 0
 quit 5
end
set var r4_trigger=0x52340002
''')
