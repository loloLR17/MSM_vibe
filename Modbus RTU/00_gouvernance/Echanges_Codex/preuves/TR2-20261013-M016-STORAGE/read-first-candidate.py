import hardware as h
from pathlib import Path
f=h.P/'readonly-build/tr2-special-commissioning_readonly-NON-CUMULATIVE.elf'
h.debug('candidate-first-chunk',f,h.IDENTITY+"""set variable tr2_commission_sd_read_lba=16384
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
p tr2_commission_sd_read_result
p tr2_commission_sd_read_done
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 1
 quit 4
end
"""+'dump binary memory '+h.win(h.P/'candidate-first64k.bin')+' tr2_commission_sd_chunk tr2_commission_sd_chunk+65536\n')
print((h.P/'candidate-first-chunk.log').read_text())
