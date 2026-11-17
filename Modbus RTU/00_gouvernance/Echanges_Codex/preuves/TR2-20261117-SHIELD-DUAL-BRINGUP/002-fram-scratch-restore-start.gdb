set pagination off
set confirm off
set print elements 24
set remotetimeout 10
target extended-remote localhost:23341
x/3wx 0x0BFA0700
if *(unsigned int *)0xBFA0700 != 0x001B0046 || *(unsigned int *)0xBFA0704 != 0x38425018 || *(unsigned int *)0xBFA0708 != 0x20333157
 disconnect
 quit 3
end
p probe
p probe_card.writes
if probe.sequence != 2 || probe.command != 2 || probe.error != 202 || probe.done_mask != 2 || probe_command || probe_arm || probe_writer.active || probe_sd_uncertain || probe.fram_mismatches
 disconnect
 quit 4
end
set var probe_export_confirmed=1
set var probe_arm=0x53414645
set var probe_command=5

detach
quit
