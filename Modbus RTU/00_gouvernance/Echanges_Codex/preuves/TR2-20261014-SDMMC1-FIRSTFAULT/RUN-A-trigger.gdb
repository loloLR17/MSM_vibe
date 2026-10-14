set pagination off
set confirm off
set print elements 0
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p ff_result
if ff_result.magic != 0x46303133 || ff_result.profile != 1 || ff_result.stage != 1 || ff_result.run_count != 0 || ff_result.trigger != 0
 quit 4
end
set ff_result.trigger = 0x52454144

detach
quit
