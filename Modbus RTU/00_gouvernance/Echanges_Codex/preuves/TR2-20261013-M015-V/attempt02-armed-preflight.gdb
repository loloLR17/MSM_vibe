set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p tr2_production_stage
p tr2_production_result
p production_runtime.recovery
p cooperative
p tr2_fifo_state
p bench_ready
p g_serial.tx_busy
p tr2_irq_observer.rx_callback
x/3wx 0xE000E100
x/2wx 0xE000ED28
x/wx 0x46020CE8
x/6wx 0x46002000
if tr2_production_stage != 2 || tr2_production_result != 0 || production_runtime.recovery.generation != 12 || bench_ready != 1 || cooperative.state != 1 || cooperative.who != 123 || tr2_fifo_fault != 0
 quit 4
end
if g_serial.tx_busy || g_serial.tx_fault || g_serial.uart.ErrorCode || g_serial.event_head != g_serial.event_tail || tr2_irq_observer.rx_callback != 0 || tr2_irq_observer.events_dropped != 0
 quit 5
end
if SystemCoreClock != 160000000 || (*(unsigned int *)0x46020CE8 & 0x18) != 0x10 || (*(unsigned int *)0x46002008 & 0x70000000) != 0x10000000 || (*(unsigned int *)0xE000E104 & 0x20000)==0 || (*(unsigned int *)0xE000E108 & 4)==0
 quit 7
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/fram-before-capture.bin official_words (official_words+262144)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/persistent-cumulative-before.bin production_runtime.candidate (production_runtime.candidate+51914)

detach
quit
