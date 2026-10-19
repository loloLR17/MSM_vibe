set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
bt
p r4u_result
p work
p engine.state
p engine.last
p engine.has_active
p owner.acquisition.fifo.running
if r4u_result.error != 6 || !work.deadline_missed || work.max_service_us < 800000 || owner.acquisition.fifo.running || engine.has_active
 quit 10
end

detach
quit
