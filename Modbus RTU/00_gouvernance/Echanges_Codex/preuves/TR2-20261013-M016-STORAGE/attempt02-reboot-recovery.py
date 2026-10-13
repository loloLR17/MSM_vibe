import hardware as h
import time
elf=h.P/'qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf'
h.debug('attempt02-software-reset-after-failed-diagnostic',elf,h.IDENTITY+'monitor reset\n')
time.sleep(30)
h.debug('attempt02-reboot-recovery',elf,h.IDENTITY+"""p tr2_commission_stage
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
"""+'dump binary memory '+h.win(h.P/'mbr-prefix-after-attempts.bin')+' tr2_commission_sd_prefix tr2_commission_sd_prefix+8192\n')
print((h.P/'attempt02-reboot-recovery.log').read_text())
