set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p tr2_commission_stage
p tr2_commission_fram_result
p tr2_commission_verify_result
p tr2_commission_sd_result
p tr2_commission_sd_prefix_result
p storage_qual_stage
p storage_qual_result
p storage_qual_scan_nonzero
p storage_qual_recovery_status
p storage_qual_recovery_bytes
p storage_qual_configuration_available
p/x hsd2.ErrorCode
p/x hsd2.Instance->CLKCR
if tr2_commission_sd_prefix_result != 0
 quit 6
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\mbr-prefix-after-attempts.bin tr2_commission_sd_prefix tr2_commission_sd_prefix+8192

detach
quit
