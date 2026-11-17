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
p/x SPI1->CFG1
if probe.stage != 1 || probe.sequence || probe_command || probe_arm || probe_card.commands || probe_card.writes || probe_writer.active
 disconnect
 quit 4
end
set var probe_command=1

detach
quit
