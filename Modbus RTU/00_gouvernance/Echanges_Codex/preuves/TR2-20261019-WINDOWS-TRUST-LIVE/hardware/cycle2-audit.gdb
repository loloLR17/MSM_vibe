set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if owner.acquisition.fifo.running || engine.has_active
 quit 9
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
p server.captured
p server.queue_full
p server.transport_errors
p tr2_irq_observer
p r4r_first_value
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-windows-trust-live-resume\hardware\cycle2-audit-xyz.bin r4r_xyz r4r_xyz+4096
set $a=(int)iis3dwb_read_reg(&owner.acquisition.fifo.ctx,15,baseline,1)
set $b=(int)iis3dwb_read_reg(&owner.acquisition.fifo.ctx,16,baseline+1,1)
set $c=(int)iis3dwb_read_reg(&owner.acquisition.fifo.ctx,10,baseline+2,1)
set $d=(int)iis3dwb_read_reg(&owner.acquisition.fifo.ctx,25,baseline+3,1)
x/4bx baseline
if $a != 0 || $b != 0 || $c != 0 || $d != 0 || baseline[0] != 0x7B || (baseline[1]&0xE0) != 0 || (baseline[2]&7) != 0 || (baseline[3]&0x20) != 0
 quit 10
end
set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
if $rr != 0
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-windows-trust-live-resume\hardware\cycle2-audit-fram.bin baseline baseline+262144

detach
quit
