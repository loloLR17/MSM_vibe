set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
printf "GATE stage=%u result=%u errors=%u gen=%u uart=%u tx=%u tick=%u\n", tr2_production_stage, tr2_production_result, tr2_production_poll_errors, production_runtime.recovery.generation, g_serial.uart.ErrorCode, g_serial.tx_busy, uwTick
p production_runtime.application.runtime.configuration_service.recovery_status
x/3wx 0xE000E100
x/2wx 0xE000ED28

detach
quit
