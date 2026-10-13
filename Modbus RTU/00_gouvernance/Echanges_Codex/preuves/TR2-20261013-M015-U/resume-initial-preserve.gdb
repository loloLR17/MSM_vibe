set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
p tr2_production_stage
p tr2_production_result
p production_runtime.recovery
p production_runtime.application.runtime.configuration_service.recovery_status
x/3wx 0xE000E100
x/2wx 0xE000ED28
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/flash-before.bin 0x08000000 0x08200000
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/flash-before-second.bin 0x08000000 0x08200000
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/ram-before.bin 0x20000000 0x200C0000
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/uid.bin 0x0BFA0700 0x0BFA070C

detach
quit
