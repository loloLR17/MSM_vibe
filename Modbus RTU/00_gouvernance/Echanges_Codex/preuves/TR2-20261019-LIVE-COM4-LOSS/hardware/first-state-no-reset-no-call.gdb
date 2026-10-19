set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
info registers
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
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-live-com4-loss-diag\hardware\flash-initial-prefix.bin 0x08000000 0x8023e50
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-live-com4-loss-diag\hardware\baseline-RAM-initial.bin baseline baseline+262144

detach
quit
