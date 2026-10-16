from hardware import *
elf=P.parent/'probe-build/tr2-r4r-physical-time-NON-PRODUCTION.elf'
debug('r4r-interruption-state',elf,IDENTITY+'''p r4r_result
p r4_result
p owner.acquisition.fifo.running
p owner.acquisition.fifo.stopped
p owner.acquisition.fifo.error
p owner.acquisition.fifo.stop_error
''')
