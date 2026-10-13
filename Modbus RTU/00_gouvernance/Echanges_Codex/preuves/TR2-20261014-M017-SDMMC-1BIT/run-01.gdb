set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if load_stage != 2 || load_error != 0 || load_generated != 0 || load_written != 0 || load_trigger != 0
 quit 4
end
hbreak load_terminal
set load_trigger=0x4C314C44
continue
printf "METRIC load_stage=%u\n", load_stage
printf "METRIC load_error=%u\n", load_error
printf "METRIC load_phase=%u\n", load_phase
printf "METRIC load_transfer_phase=%u\n", load_transfer_phase
printf "METRIC load_generated=%u\n", load_generated
printf "METRIC load_produced=%u\n", load_produced
printf "METRIC load_consumed=%u\n", load_consumed
printf "METRIC load_backlog_max=%u\n", load_backlog_max
printf "METRIC load_overflow=%u\n", load_overflow
printf "METRIC load_lost=%u\n", load_lost
printf "METRIC load_producer_done=%u\n", load_producer_done
printf "METRIC load_producer_enabled=%u\n", load_producer_enabled
printf "METRIC load_written=%u\n", load_written
printf "METRIC load_written_logical=%u\n", load_written_logical
printf "METRIC load_verified=%u\n", load_verified
printf "METRIC load_verified_logical=%u\n", load_verified_logical
printf "METRIC load_write_chunks=%u\n", load_write_chunks
printf "METRIC load_read_chunks=%u\n", load_read_chunks
printf "METRIC load_mismatches=%u\n", load_mismatches
printf "METRIC load_first_mismatch=%u\n", load_first_mismatch
printf "METRIC load_first_io_error=%u\n", load_first_io_error
printf "METRIC load_first_cmd_error=%u\n", load_first_cmd_error
printf "METRIC load_max_producer_gap_us=%u\n", load_max_producer_gap_us
printf "METRIC load_generation_us=%u\n", load_generation_us
printf "METRIC load_write_elapsed_us=%u\n", load_write_elapsed_us
printf "METRIC load_verify_elapsed_us=%u\n", load_verify_elapsed_us
printf "METRIC load_clock_hz=%u\n", load_clock_hz
printf "METRIC load_kernel_hz=%u\n", load_kernel_hz
printf "METRIC load_clkcr=%u\n", load_clkcr
printf "METRIC load_bus_card=%u\n", load_bus_card
printf "METRIC load_blocks=%u\n", load_blocks
printf "METRIC load_dma_error=%u\n", load_dma_error
printf "METRIC load_hal_return=%u\n", load_hal_return
printf "METRIC load_last_r1=%u\n", load_last_r1
printf "METRIC load_cmd_error=%u\n", load_cmd_error
printf "METRIC load_producer_cycles=%u\n", load_producer_cycles
printf "METRIC load_producer_max_cycles=%u\n", load_producer_max_cycles
p/x load_cid
p/x hsd2.ErrorCode
x/2wx 0xE000ED28
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M017-20261014\run-01-total.bin load_write_latency_us load_write_latency_us+4096
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M017-20261014\run-01-data.bin load_dma_latency_us load_dma_latency_us+4096
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M017-20261014\run-01-ready.bin load_ready_latency_us load_ready_latency_us+4096

detach
quit
