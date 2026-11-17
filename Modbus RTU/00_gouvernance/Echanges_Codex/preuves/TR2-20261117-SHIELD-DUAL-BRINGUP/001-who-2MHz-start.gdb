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
p/x probe_card.cid
p/x probe_card.csd
if probe.sequence != 2 || probe.command != 2 || probe.error || probe.done_mask != 4 || probe_command || probe_arm || probe_card.writes || !probe_card.initialized || probe_card.failed
 disconnect
 quit 4
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/001-sd-cid.bin probe_card.cid (probe_card.cid+16)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/001-sd-csd.bin probe_card.csd (probe_card.csd+16)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/001-sd-sector0-readonly.bin probe_sd_sector0 (probe_sd_sector0+512)
set var probe_command=3

detach
quit
