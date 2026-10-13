import hardware as h
from pathlib import Path
import subprocess,time
sha=subprocess.check_output(['git','rev-parse','HEAD'],text=True).strip()
f=h.P/'readonly-build/tr2-special-commissioning_readonly-NON-CUMULATIVE'
h.flash('flash-readonly-probe',Path(str(f)+'.bin'),sha)
h.debug('reset-irq-after-programmer',Path(str(f)+'.elf'),h.IDENTITY+'monitor reset\n')
time.sleep(4)
h.debug('readonly-inventory',Path(str(f)+'.elf'),h.IDENTITY+"""p tr2_commission_stage
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
print((h.P/'readonly-inventory.log').read_text())
