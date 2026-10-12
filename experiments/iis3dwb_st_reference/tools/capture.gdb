# TEMPLATE ONLY. Do not source until the SECOND board and its GDB server
# identity are confirmed. No target connection, flash, trigger or reset here.
set pagination off
set print elements 0
p bench_uid
p bench_core_hz
p bench_result
p bench_trace_count
p bench_payload_count
# Use new, absolute, exclusive evidence paths, not these placeholders:
# dump binary memory <result.bin> &bench_result ((char *)&bench_result)+sizeof(bench_result)
# dump binary memory <traces.bin> bench_trace ((char *)bench_trace)+bench_trace_count*sizeof(BenchTrace)
# dump binary memory <payload.bin> bench_payload bench_payload+bench_payload_count
# dump binary memory <fifo.bin> bench_result.words ((char *)bench_result.words)+bench_result.count*7
# dump binary memory <flash.bin> 0x08000000 0x08200000
# Trigger (separate authorized session only): set var bench_command=0x4D31354B
# Stop (during an authorized run only): set var bench_stop=1
# Always detach/quit and check server exit after the selected observation.
