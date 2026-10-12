set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
p bench_ready
p bench_uid
p bench_core_hz
p bench_result
x/2wx 0xE000ED28
if bench_ready != 1
  quit 2
end
if bench_uid[0] != 0x001B0046 || bench_uid[1] != 0x38425018 || bench_uid[2] != 0x20333157
  quit 3
end
if bench_core_hz != 16000000
  quit 4
end
set var bench_command=0x4D31354B

detach
quit
