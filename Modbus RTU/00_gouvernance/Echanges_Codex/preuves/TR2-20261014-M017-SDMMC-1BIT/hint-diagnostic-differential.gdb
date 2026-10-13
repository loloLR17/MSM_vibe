set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if load_stage != 2 || load_error != 0 || load_clock_hz != 10000000 || load_bus_card != 0 || diag_done != 0
 quit 4
end
hbreak load_main.c:237
set load_trigger=0x4D313744
continue
p diag_done
p diag_error
p diag_mismatches
p diag_latency_us
p diag_data_us
p diag_ready_us
p load_preerase_count
p/x load_preerase_error
p/x load_preerase_r1
p/x hsd2.ErrorCode
p/x load_cid
p/x load_clkcr
p load_clock_hz

detach
quit
