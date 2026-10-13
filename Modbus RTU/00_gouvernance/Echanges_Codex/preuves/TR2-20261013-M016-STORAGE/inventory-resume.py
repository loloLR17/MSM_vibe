import hardware as h
from pathlib import Path
f=h.P/'readonly-build/tr2-special-commissioning_readonly-NON-CUMULATIVE'
h.debug('readonly-inventory-with-symbols',Path(str(f)+'.elf'),h.IDENTITY+"""p tr2_commission_stage
p tr2_commission_fram_result
p tr2_commission_verify_result
p tr2_commission_sd_result
p tr2_commission_sd_prefix_result
p tr2_commission_sd_blocks
p hsd2.CID
p hsd2.CSD
p hsd2.ErrorCode
p tr2_commission_sensor_whoami
if tr2_commission_stage != 5 || tr2_commission_fram_result || tr2_commission_verify_result || tr2_commission_sd_result || tr2_commission_sd_prefix_result
 quit 4
end
"""+'dump binary memory '+h.win(h.P/'current-sd-prefix.bin')+' tr2_commission_sd_prefix tr2_commission_sd_prefix+8192\n'+'dump binary memory '+h.win(h.P/'current-fram.bin')+' tr2_commission_fram_backup tr2_commission_fram_backup+262144\n')
print((h.P/'readonly-inventory-with-symbols.log').read_text())
