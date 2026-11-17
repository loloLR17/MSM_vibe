set pagination off
set confirm off
set print elements 24
set remotetimeout 10
target extended-remote localhost:23341
x/3wx 0x0BFA0700
if *(unsigned int *)0xBFA0700 != 0x004A003D || *(unsigned int *)0xBFA0704 != 0x38425018 || *(unsigned int *)0xBFA0708 != 0x20333157
 disconnect
 quit 3
end
p probe
p probe_command
p/x fram_spi.ErrorCode
p/x GPIOD->ODR
if probe.stage == 3 && probe.error == 0 && probe.fram_mismatches == 0
 printf "FRAM_CRC_HEX=%08lx\n", probe.fram_crc
 dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/001-fram-original-262144.bin probe_fram_snapshot (probe_fram_snapshot+262144)
end
x/2wx 0xE000ED28

detach
quit
