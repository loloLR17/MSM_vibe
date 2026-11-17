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
p probe_sd_path
if probe.stage != 3 || probe.sequence != 4 || probe.command != 6 || probe.error || probe.sd_file_verified != 1 || probe.done_mask != 68 || probe_command || probe_arm || probe_card.failed || probe_sd_uncertain || probe_writer.active || probe_writer.uncertain
 disconnect
 quit 4
end
set var probe_arm=0x53414645
set var probe_command=7

detach
quit
