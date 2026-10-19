from hardware import *
e=Path('/home/lolo/dev/msm/reports/TR2-uxl2-physical/firmware/tr2-live-operator-NON-PRODUCTION.elf')
debug('first-state-no-reset-no-call',e,IDENTITY+'''info registers
bt
p r4u_result
p work
p owner.acquisition.fifo.running
p owner.acquisition.fifo.error
p owner.acquisition.fifo.stop_error
p engine.state
p engine.last
p engine.has_active
p store.generation
p live.generation
p server.transport_errors
p server.queue_full
p tr2_irq_observer.error_callback
p tr2_irq_observer.events_dropped
p g_serial
x/6wx 0xE000ED24
x/3wx 0xE000E100
x/3wx 0xE000E200
x/8wx 0x40013800
'''+f'dump binary memory {win(P/"flash-initial-prefix.bin")} 0x08000000 {hex(0x08000000+147024)}\n'+f'dump binary memory {win(P/"baseline-RAM-initial.bin")} baseline baseline+262144\n')
