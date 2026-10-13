set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p SystemCoreClock
p tr2_production_stage
p tr2_production_result
p production_runtime.recovery
p cooperative.state
p cooperative.cleanup_result
p cooperative.cleanup
p tr2_fifo_state
x/3wx 0xE000E100
x/2wx 0xE000ED28
if tr2_production_stage != 2 || tr2_production_result != 0 || production_runtime.recovery.generation != 12 || (cooperative.state != 4 && cooperative.state != 5)
 quit 4
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/flash-before.bin 0x08000000 0x08200000
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/persistent-before.bin production_runtime.candidate (production_runtime.candidate+51914)

detach
quit
