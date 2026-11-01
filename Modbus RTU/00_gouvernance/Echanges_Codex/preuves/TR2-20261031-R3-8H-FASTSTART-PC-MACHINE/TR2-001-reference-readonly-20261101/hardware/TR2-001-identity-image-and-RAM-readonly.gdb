set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-001-reference-readonly-20261101/hardware/TR2-001-flash-prefix.bin 0x08000000 0x8024dc0
p/x $pc
bt 5
p r4u_result
p engine.last
p configuration.active
p work.depth
p work.servicing
p runtime.main_busy
p runtime.servicing
p owner.acquisition.fifo.running
p owner.acquisition.fifo.error
p owner.acquisition.fifo.stop_error
p/x *(unsigned int*)0x46002008
p/x *(unsigned int*)0x4600200c

detach
quit
