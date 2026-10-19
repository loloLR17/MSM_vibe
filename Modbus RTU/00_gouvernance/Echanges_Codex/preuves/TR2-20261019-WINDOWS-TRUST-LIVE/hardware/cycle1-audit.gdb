set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4u_result
p r4_result
p work
p owner.acquisition.fifo.running
p owner.acquisition.fifo.total_xyz
p owner.acquisition.fifo.total_ts
p owner.acquisition.fifo.parser
p owner.acquisition.fifo.error
p owner.acquisition.fifo.stop_error
p engine.state
p engine.last
p store.generation
p store.epoch
p live.generation
p metric
p event_time
p server
p tr2_irq_observer
p r4r_first_value
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-windows-trust-live-resume\hardware\cycle1-audit-xyz.bin r4r_xyz r4r_xyz+4096
set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
if $rr != 0
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-windows-trust-live-resume\hardware\cycle1-audit-fram.bin baseline baseline+262144

detach
quit
