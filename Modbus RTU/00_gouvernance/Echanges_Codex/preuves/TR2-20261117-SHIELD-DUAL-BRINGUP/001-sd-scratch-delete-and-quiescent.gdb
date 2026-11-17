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
p probe_writer
p probe_card.error
p probe_card.writes
p probe_card.crc_errors
p probe_card.timeouts
p probe_sd_uncertain
p/x GPIOF->ODR
p/x GPIOC->ODR
p/x GPIOD->ODR
p/x GPIOG->ODR
x/2wx 0xE000ED28
if probe.stage != 3 || probe.command != 7 || probe.sequence != 5 || probe.error || probe.sd_file_deleted != 1 || probe_command || probe_arm || probe_writer.active || probe_writer.uncertain || probe_sd_uncertain || probe_card.failed
 disconnect
 quit 5
end

detach
quit
