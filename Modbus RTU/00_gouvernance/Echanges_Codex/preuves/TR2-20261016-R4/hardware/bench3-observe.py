from hardware import *
elf=P.parent/'bench-build/tr2-r4-qualification-NON-PRODUCTION.elf'
debug('bench3-readonly-result',elf,IDENTITY+'''p r4_result
p work
p owner.acquisition.fifo
p bench_log_refused
p bench_preclose_overflow
x/2wx 0xE000E100
''')
