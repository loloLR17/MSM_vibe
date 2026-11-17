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
p probe_card
if probe.sequence != 3 || probe.command != 3 || probe.error != 301 || probe.done_mask != 4 || probe_command || probe_arm || !probe_card.initialized || probe_card.failed || probe_card.writes || probe_writer.active || probe_sd_uncertain
 disconnect
 quit 4
end
set var probe_arm=0x53414645
set var probe_command=6

detach
quit
