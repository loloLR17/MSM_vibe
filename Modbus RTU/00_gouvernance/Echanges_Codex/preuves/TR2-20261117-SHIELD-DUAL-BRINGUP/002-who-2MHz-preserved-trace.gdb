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
p bench_trace_count
p bench_trace[0]
p bench_preclose_count
p bench_preclose[0]
p bench_command_rx[0]
p/x GPIOC->IDR
p/x GPIOC->ODR
p/x SPI3->CFG1
p/x SPI3->CFG2
p probe_writer.active
p probe_card.writes
p probe_sd_uncertain
x/2wx 0xE000ED28
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/002-probe-who-2MHz-payload.bin bench_payload (bench_payload+1)
if probe.sequence != 4 || probe.command != 3 || probe.error != 301 || probe_command || probe_arm || bench_trace_count != 1 || probe_writer.active || probe_writer.uncertain || probe_card.writes
 disconnect
 quit 5
end

detach
quit
