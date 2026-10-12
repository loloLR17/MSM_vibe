set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
p bench_result
p official_packets
p official_ctrl10
p bench_core_hz
p bench_uid
x/2wx 0xE000ED28

detach
quit
