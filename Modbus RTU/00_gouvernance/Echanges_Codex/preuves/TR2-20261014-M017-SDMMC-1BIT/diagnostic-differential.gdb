set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
hbreak differential
set load_trigger=0x4D313744
continue
finish
p diag_done
p diag_error
p diag_mismatches
p diag_latency_us
p diag_data_us
p diag_ready_us
p/x hsd2.ErrorCode
p/x load_last_r1
x/2wx 0xE000ED28

detach
quit
