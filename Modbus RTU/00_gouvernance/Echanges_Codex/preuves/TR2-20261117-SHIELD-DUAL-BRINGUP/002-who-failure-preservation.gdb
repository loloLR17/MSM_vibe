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
p bench_trace_count
p bench_payload_count
p bench_trace[0]
p/x bench_command_rx[0]
p bench_preclose[0]
p bench_preclose[1]
p/x GPIOC->MODER
p/x GPIOC->AFR[1]
p/x GPIOC->IDR
p/x GPIOC->ODR
p/x SPI3->CR1
p/x SPI3->CFG1
p/x SPI3->CFG2
p/x SPI3->SR
p r3_card.commands
p r3_card.writes
p durable_status.io_writes
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/002-WHO-traces.bin bench_trace ((char*)bench_trace)+sizeof(bench_trace)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/002-WHO-payload.bin bench_payload ((char*)bench_payload)+bench_payload_count

detach
quit
