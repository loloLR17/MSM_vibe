set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
if *(unsigned int*)0x0BFA0700 != 0x004A003D || *(unsigned int*)0x0BFA0704 != 0x38425018 || *(unsigned int*)0x0BFA0708 != 0x20333157
 echo UNSAFE_AUDIT_REFUSED\n
 detach
 quit 7
end
if owner.acquisition.fifo.running || engine.has_active || r4u_result.error != 0 || engine.state != LIVE_READY || engine.last.status != 4 || engine.last.result != 0 || engine.last.request.command.identity.command_code != 14 || engine.last.request.epoch != 5 || engine.last.request.command.transaction_id != 25
 echo UNSAFE_AUDIT_REFUSED\n
 detach
 quit 7
end
tbreak live_runtime_step
continue
if owner.acquisition.fifo.running || engine.has_active || r4u_result.error != 0 || engine.state != LIVE_READY || engine.last.status != 4 || engine.last.result != 0 || engine.last.request.command.identity.command_code != 14 || engine.last.request.epoch != 5 || engine.last.request.command.transaction_id != 25
 echo UNSAFE_AUDIT_REFUSED\n
 detach
 quit 7
end
if work.depth != 0 || work.servicing || runtime.main_busy || runtime.servicing
 echo UNSAFE_AUDIT_REFUSED\n
 detach
 quit 7
end
bt
p work
p r4u_result
p engine.last
p store.generation
p live.generation
p owner.acquisition.fifo.error
p owner.acquisition.fifo.stop_error
p configuration.active
p owner.acquisition.fifo.full_scale
p r4r_first_value
p r4r_result
p r4_result
p tr2_irq_observer
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-demo-metrology-ux\hardware\xyz.bin r4r_xyz r4r_xyz+4096
set $rr=(int)owner.acquisition.fifo.ctx.read_reg(owner.acquisition.fifo.ctx.handle,0x10,baseline,1)
p $rr
x/1bx baseline
if $rr != 0 || (baseline[0] & 0x0C) != 0
 echo UNSAFE_AUDIT_REFUSED\n
 detach
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-demo-metrology-ux\hardware\ctrl1-xl.bin baseline baseline+1
set $rr=(int)owner.acquisition.fifo.ctx.read_reg(owner.acquisition.fifo.ctx.handle,0x0F,baseline,1)
p $rr
x/1bx baseline
if $rr != 0 || baseline[0] != 0x7B
 echo UNSAFE_AUDIT_REFUSED\n
 detach
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-demo-metrology-ux\hardware\who-am-i.bin baseline baseline+1
set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
if $rr != 0
 echo UNSAFE_AUDIT_REFUSED\n
 detach
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-demo-metrology-ux\hardware\fram.bin baseline baseline+262144
p work

detach
quit
