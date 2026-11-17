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
p probe_command
p probe_arm
p probe_writer
p probe_card.writes
p/x GPIOD->ODR
p/x GPIOC->ODR
p/x GPIOF->ODR
if probe.command != 5 || probe.stage != 3 || probe.error || probe.sequence != 3 || probe.fram_restore_mismatches || probe_arm || probe_command || probe_card.writes || probe_writer.active
 disconnect
 quit 5
end

detach
quit
