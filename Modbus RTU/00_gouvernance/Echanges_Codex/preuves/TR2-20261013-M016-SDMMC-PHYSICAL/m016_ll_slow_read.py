from pathlib import Path
import sys
P=Path(Path('/tmp/m016-sd-physical-proof').read_text());sys.path.insert(0,str(P));from hardware import *
b=Path('/home/lolo/dev/msm/reports/M016-sdmmc-reference-two-node-build')
debug('readonly-4MHz-actual-divider',b/'sdmmc_reference.elf',IDENTITY+'''p hsd2.State
p/x hsd2.Init
p/x hsd2.Instance->CLKCR
set hsd2.Init.ClockDiv=6
p SDMMC_Init(hsd2.Instance,hsd2.Init)
p/x hsd2.Instance->CLKCR
if (hsd2.Instance->CLKCR & 0x3ff) != 6 || (hsd2.Instance->CLKCR & 0xc000) != 0x4000
 quit 4
end
p ready()
p transfer(0)
p/x hsd2.ErrorCode
p/x bench_dma_error
p/x bench_last_r1
p/x bench_cmd13_error
p/x hsd2.Instance->CLKCR
'''+ 'dump binary memory '+win(P/'readonly-4MHz-actual-rx.bin')+' rx rx+4096\n',timeout=40)
print((P/'readonly-4MHz-actual-divider.log').read_text(errors='replace'))
