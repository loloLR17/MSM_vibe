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
p tr2_production_poll_errors
p production_runtime.recovery
p production_runtime.persistent_identity
p production_runtime.application.runtime.configuration_service.recovery_status
p g_serial.tx_busy
p g_serial.tx_fault
p g_serial.uart.ErrorCode
x/3wx 0xE000E100
x/2wx 0xE000ED28
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/qualification/persistent-candidate-after.bin production_runtime.candidate (production_runtime.candidate+51914)

detach
quit
