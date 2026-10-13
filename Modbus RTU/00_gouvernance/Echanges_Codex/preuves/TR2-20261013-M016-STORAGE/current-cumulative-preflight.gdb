set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p tr2_production_stage
p tr2_production_result
p cooperative.state
p cooperative.cleanup
p cooperative_fram_state
p g_serial.tx_busy
p g_serial.tx_fault
p production_runtime.recovery
if tr2_production_stage != 2 || tr2_production_result != 0 || g_serial.tx_busy || g_serial.tx_fault
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\current-cumulative.bin 0x08000000 0x080297DC

detach
quit
