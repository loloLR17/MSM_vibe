set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4u_result
p engine.last
p engine.state
p owner.acquisition.fifo.running
p store.generation
p live.generation
if r4u_result.stage != 7 || r4u_result.error != 0 || owner.acquisition.fifo.running || store.generation != 86 || live.generation != 99
 quit 8
end

detach
quit
