set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p bench_stage
p bench_phase
p/x bench_error
p/x bench_cid
p bench_blocks
p bench_kernel_hz
p bench_clock_hz
p/x bench_clkcr
p/x bench_bus_width
p/x bench_last_r1
p/x bench_cmd13_error
p hsd2.SdCard
x/2wx 0xE000ED28

detach
quit
