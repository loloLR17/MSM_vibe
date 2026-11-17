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
if probe.stage != 3 || probe.sequence != 1 || probe.command != 1 || probe.error || probe.fram_mismatches || probe.done_mask != 2 || probe_command || probe_arm || probe_card.commands || probe_card.writes || probe_writer.active
 disconnect
 quit 4
end
set var probe_export_confirmed=1
set var probe_arm=0x53414645
set var probe_command=5

detach
quit
