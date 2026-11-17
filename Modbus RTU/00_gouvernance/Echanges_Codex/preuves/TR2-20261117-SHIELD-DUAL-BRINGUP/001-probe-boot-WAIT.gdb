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
p probe_card.commands
p probe_card.writes
p probe_writer.active
p/x GPIOG->ODR
p/x GPIOD->ODR
p/x GPIOC->ODR
p/x GPIOF->ODR
x/2wx 0xE000ED28
if probe.stage != 1 || probe.sequence != 0 || probe_command || probe_arm || probe_card.commands || probe_card.writes || probe_writer.active
 disconnect
 quit 5
end
set var probe_sd_path="0:/T1700101.TMP"
p probe_sd_path

detach
quit
