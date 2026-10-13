set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if storage_qual_stage != 2 || storage_qual_result != 0 || storage_qual_scan_nonzero != 0 || storage_qual_recovery_status[0] != 1 || storage_qual_recovery_status[1] != 1
 quit 4
end
set variable storage_qual_trigger=0x4D313657

detach
quit
