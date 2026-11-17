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
if probe.sequence != 3 || probe.command != 5 || probe.error || probe_command || probe_arm || probe_writer.active || probe_sd_uncertain
 disconnect
 quit 4
end
set var probe_command=3

detach
quit
