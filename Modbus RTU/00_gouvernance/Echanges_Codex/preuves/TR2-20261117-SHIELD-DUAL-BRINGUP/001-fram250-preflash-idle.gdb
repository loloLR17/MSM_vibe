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
p probe_writer.active
p probe_writer.uncertain
p probe_sd_uncertain
if probe.stage != 3 || probe.command != 7 || probe.sequence != 5 || probe.error || probe_command || probe_arm || probe_writer.active || probe_writer.uncertain || probe_sd_uncertain || !probe.sd_file_deleted
 disconnect
 quit 4
end

detach
quit
