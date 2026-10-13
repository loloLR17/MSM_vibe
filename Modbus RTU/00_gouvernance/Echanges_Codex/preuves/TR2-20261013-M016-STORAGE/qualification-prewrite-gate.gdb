set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p storage_qual_stage
p storage_qual_result
p storage_qual_scan_nonzero
p storage_qual_recovery_status
p storage_qual_recovery_bytes
p tr2_commission_stage
p tr2_commission_fram_result
p tr2_commission_verify_result
p hsd2.CID
p hsd2.ErrorCode

detach
quit
