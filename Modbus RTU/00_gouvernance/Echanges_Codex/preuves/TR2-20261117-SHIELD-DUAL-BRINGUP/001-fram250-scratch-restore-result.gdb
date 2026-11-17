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
p probe_command
p probe_arm
p probe_writer.active
p probe_writer.uncertain
p probe_card.commands
p probe_card.writes
p probe_sd_uncertain
p/x SPI1->CFG1
p/x GPIOC->ODR
p/x GPIOD->ODR
p/x GPIOF->ODR
p/x GPIOG->ODR
x/2wx 0xE000ED28
if probe.stage != 3 || probe.sequence != 2 || probe.command != 5 || probe.error || probe.fram_restore_mismatches || probe_command || probe_arm || probe.done_mask != 34 || probe_writer.active || probe_writer.uncertain || probe_card.commands || probe_card.writes
 disconnect
 quit 5
end

detach
quit
