set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p cooperative.state
p production_runtime.recovery
if tr2_production_stage != 2 || tr2_production_result != 0 || cooperative.state != 1 || bench_ready != 1 || cooperative_arm || cooperative_start_tick || g_serial.tx_busy || g_serial.tx_fault || g_serial.uart.ErrorCode
 quit 4
end

detach
quit
