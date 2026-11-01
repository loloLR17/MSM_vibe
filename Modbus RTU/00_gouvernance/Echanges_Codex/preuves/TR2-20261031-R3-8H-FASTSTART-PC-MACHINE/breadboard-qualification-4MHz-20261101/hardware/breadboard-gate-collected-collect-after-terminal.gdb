set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result
p r3_writer
p witness_sequence
p witness_errors
if r3_result.stage!=4 || r3_result.error || r3_result.run!=3 || r3_command || fifo.running || r3_writer.active || q.count || witness_errors || r3_clock_fault || !(*(unsigned int*)0x42021414 &0x1000)
 quit 7
end
if (durable_busy_distributions+2) > durable_busy_distributions
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/busy-distributions.bin durable_busy_distributions (durable_busy_distributions+2)
end
if (&durable_single_busy_max+1) > &durable_single_busy_max
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/single-busy-max.bin &durable_single_busy_max (&durable_single_busy_max+1)
end
if (&durable_status+1) > &durable_status
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/durable-status.bin &durable_status (&durable_status+1)
end
if (durable_footer+512) > durable_footer
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/durable-footer.bin durable_footer (durable_footer+512)
end
if 0x46002010 > 0x46002008
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/spi3-config.bin 0x46002008 0x46002010
end
if 0x40003810 > 0x40003808
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/spi2-config.bin 0x40003808 0x40003810
end
if (&r3_result+1) > &r3_result
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/result.bin &r3_result (&r3_result+1)
end
if (&r3_card+1) > &r3_card
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/card.bin &r3_card (&r3_card+1)
end
if (r3_records+r3_result.trace_count) > r3_records
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/records.bin r3_records (r3_records+r3_result.trace_count)
end
if (r3_distributions+4) > r3_distributions
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/distributions.bin r3_distributions (r3_distributions+4)
end
if (r3_sd_records+(r3_result.sd_completed<2048?r3_result.sd_completed:2048)) > r3_sd_records
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/sd-records.bin r3_sd_records (r3_sd_records+(r3_result.sd_completed<2048?r3_result.sd_completed:2048))
end
if (r3_sd_distributions+2) > r3_sd_distributions
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/sd-distributions.bin r3_sd_distributions (r3_sd_distributions+2)
end
if (r3_brackets+r3_result.cal_brackets) > r3_brackets
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/brackets.bin r3_brackets (r3_brackets+r3_result.cal_brackets)
end
if (r3_raw+r3_result.raw_words*7) > r3_raw
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/raw.bin r3_raw (r3_raw+r3_result.raw_words*7)
end
if (r3_sources+r3_source_count) > r3_sources
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/sources.bin r3_sources (r3_sources+r3_source_count)
end
if (r3_outputs+r3_output_count) > r3_outputs
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/outputs.bin r3_outputs (r3_outputs+r3_output_count)
end
if (r3_source_sha+32) > r3_source_sha
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/source-sha.bin r3_source_sha (r3_source_sha+32)
end
if (r3_header+512) > r3_header
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/header.bin r3_header (r3_header+512)
end
if (bench_trace+bench_trace_count) > bench_trace
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/fifo-stop-trace.bin bench_trace (bench_trace+bench_trace_count)
end
if (bench_payload+bench_payload_count) > bench_payload
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/fifo-stop-payload.bin bench_payload (bench_payload+bench_payload_count)
end

detach
quit
