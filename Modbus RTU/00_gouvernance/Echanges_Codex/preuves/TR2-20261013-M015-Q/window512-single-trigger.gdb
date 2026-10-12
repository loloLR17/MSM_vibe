set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
p bench_ready
p bench_uid
p bench_core_hz
p bench_result
x/2wx 0xE000ED28
if bench_ready != 1 || bench_core_hz != 16000000 || bench_uid[0] != 0x001B0046 || bench_uid[1] != 0x38425018 || bench_uid[2] != 0x20333157
 quit 4
end
if bench_result.stage != 0 || bench_result.count != 0 || *(unsigned int *)0xE000ED28 != 0 || *(unsigned int *)0xE000ED2C != 0
 quit 5
end
set var bench_command=0x4D31354B

detach
quit
