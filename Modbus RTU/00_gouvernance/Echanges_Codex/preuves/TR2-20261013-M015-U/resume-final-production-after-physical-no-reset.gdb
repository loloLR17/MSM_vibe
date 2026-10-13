set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
p uwTick
p tr2_production_stage
p tr2_production_result
p tr2_production_poll_errors
p production_runtime.recovery
p production_runtime.persistent_identity
p production_runtime.application.runtime.configuration_service.recovery_status
p production_runtime.application.runtime.b1_image
p g_serial.tx_busy
p g_serial.tx_fault
p g_serial.uart.ErrorCode
p g_serial.event_head
p g_serial.event_tail
x/3wx 0xE000E100
x/2wx 0xE000ED28
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/ram-final-after-physical.bin 0x20000000 0x200C0000

detach
quit
