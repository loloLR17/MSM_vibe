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
p probe_card.writes
p probe_card.error
p probe_card.crc_errors
p probe_card.timeouts
p probe_sd_uncertain
p/x GPIOF->ODR
if probe.stage == 3 && probe.command == 6 && probe.error == 0 && probe.sd_file_verified == 1
 dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/001-sd-scratch-returned-4096.bin probe_sd_return (probe_sd_return+4096)
 dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/001-sd-scratch-expected-4096.bin probe_sd_expected (probe_sd_expected+4096)
 printf "SD_CRC_HEX=%08lx\n", probe.sd_crc
end

detach
quit
