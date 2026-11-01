set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4u_result
p r4r_result.calibration_residual_us
p work.error
p owner.acquisition.fifo.running
p diag_fifo.running
p configuration.active.config_id
p configuration.active.generation
p configuration.active.payload.full_scale_code
p engine.last
p/x *(unsigned int*)0x42020814
if r4u_result.stage!=7 || r4u_result.error || work.error || owner.acquisition.fifo.running || diag_fifo.running || configuration.active.payload.full_scale_code!=0
 quit 8
end

detach
quit
