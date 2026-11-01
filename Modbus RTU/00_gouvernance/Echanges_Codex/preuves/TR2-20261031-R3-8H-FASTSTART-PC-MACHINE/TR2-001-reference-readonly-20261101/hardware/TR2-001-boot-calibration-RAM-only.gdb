set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4u_result
p r4r_result
p diag_fifo.running
p diag_fifo.error
p diag_fifo.stop_error
p diag_fifo.total_xyz
p diag_fifo.full_scale
p diag_fifo.parser
p owner.acquisition.fifo.running
p owner.acquisition.fifo.error
p owner.acquisition.fifo.stop_error
p bench_trace_count
p bench_payload_count
p bench_preclose_count
p work.error
p configuration.active.payload.full_scale_code
if r4u_result.stage!=7 || r4u_result.error || r4r_result.error || diag_fifo.running || diag_fifo.error || diag_fifo.stop_error || owner.acquisition.fifo.running || owner.acquisition.fifo.error || owner.acquisition.fifo.stop_error || r4r_result.calibration_residual_us>100 || r4r_result.brackets<100 || r4r_result.anchors<100 || r4r_result.counter_mismatch || r4r_result.reserved_nonzero
 quit 8
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-001-reference-readonly-20261101/calibration-result.bin &r4r_result ((char*)&r4r_result)+sizeof(r4r_result)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-001-reference-readonly-20261101/calibration-brackets.bin r4r_brackets r4r_brackets+r4r_result.brackets
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-001-reference-readonly-20261101/calibration-first-raw.bin r4r_raw r4r_raw+r4r_result.raw_words*7
if bench_trace_count<=1024 && bench_payload_count<=65536
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-001-reference-readonly-20261101/boot-transport-trace.bin bench_trace bench_trace+bench_trace_count
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-001-reference-readonly-20261101/boot-transport-payload.bin bench_payload bench_payload+bench_payload_count
end

detach
quit
