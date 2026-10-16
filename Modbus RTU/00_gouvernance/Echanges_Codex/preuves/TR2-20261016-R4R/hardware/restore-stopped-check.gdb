set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4r_result.stage
p r4r_result.error
p owner.acquisition.fifo.stopped
p owner.acquisition.fifo.stop_error
p engine.indeterminate
if owner.acquisition.fifo.running || !owner.acquisition.fifo.stopped || owner.acquisition.fifo.stop_error
 quit 4
end

detach
quit
