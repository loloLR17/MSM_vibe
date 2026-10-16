set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4_result
p work
p owner.acquisition.fifo
p configuration
p engine.state
p store.epoch
p bench_log_refused
p bench_preclose_overflow
p hspi1.ErrorCode
p live.generation
p engine.last

detach
quit
