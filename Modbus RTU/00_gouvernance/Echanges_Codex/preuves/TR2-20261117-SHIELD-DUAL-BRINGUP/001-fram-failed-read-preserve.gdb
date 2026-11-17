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
if probe.sequence != 1 || probe.command != 1 || probe.error != 113 || probe.fram_mismatches != 9 || probe_command || probe_arm || probe.done_mask != 0
 disconnect
 quit 4
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/001-fram-first-read-unstable-262144.bin probe_fram_snapshot (probe_fram_snapshot+262144)

detach
quit
