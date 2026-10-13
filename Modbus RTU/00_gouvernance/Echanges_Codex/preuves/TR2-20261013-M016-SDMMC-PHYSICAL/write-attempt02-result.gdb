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
p bench_rounds
p/x bench_error
p/x bench_refusal
p/x bench_dma_error
p bench_mismatch
p bench_elapsed_ms
p bench_write_ms
p bench_read_ms
p/x bench_last_r1
p/x bench_cmd13_error
p/x bench_hal_return
p/x hsd2.ErrorCode
p/x bench_clkcr
p bench_kernel_hz
p bench_clock_hz
x/2wx 0xE000ED28
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-SDMMC-PHYSICAL-20261013T202934Z\attempt02-rx.bin rx rx+4096

detach
quit
