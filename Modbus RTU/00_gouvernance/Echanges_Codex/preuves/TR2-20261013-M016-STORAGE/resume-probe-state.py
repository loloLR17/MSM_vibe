import hardware as h
from pathlib import Path
f=h.P/'readonly-build/tr2-special-commissioning_readonly-NON-CUMULATIVE.elf'
h.debug('resume-readonly-state',f,h.IDENTITY+"""p tr2_commission_stage
p tr2_commission_sd_read_done
p tr2_commission_sd_read_trigger
p tr2_commission_sd_result
p hsd2.CID
if tr2_commission_stage != 5 || tr2_commission_sd_read_trigger != 0
 quit 4
end
"""+'dump binary memory '+h.win(h.P/'resume-readonly-flash.bin')+' 0x08000000 0x0800'+format(len((h.P/'readonly-build/tr2-special-commissioning_readonly-NON-CUMULATIVE.bin').read_bytes()),'X')+'\n')
assert (h.P/'resume-readonly-flash.bin').read_bytes()==(h.P/'readonly-build/tr2-special-commissioning_readonly-NON-CUMULATIVE.bin').read_bytes()
print('Resume target identity/readonly image confirmed')
