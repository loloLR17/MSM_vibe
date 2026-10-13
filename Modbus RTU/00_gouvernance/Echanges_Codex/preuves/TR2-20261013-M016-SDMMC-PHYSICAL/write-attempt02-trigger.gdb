set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p bench_stage
p/x bench_cid
p/x bench_bus_width
p bench_blocks
if bench_stage != 2 || bench_blocks != 62333952 || bench_bus_width != 0x4000 || bench_trigger != 0
 quit 4
end
if bench_cid[0] != 0x03534453 || bench_cid[1] != 0x44333247 || bench_cid[2] != 0x858BF092 || bench_cid[3] != 0x3C01927A
 quit 5
end
set bench_expected_cid[0]=0x03534453
set bench_expected_cid[1]=0x44333247
set bench_expected_cid[2]=0x858BF092
set bench_expected_cid[3]=0x3C01927A
set bench_trigger=0x4D313642

detach
quit
