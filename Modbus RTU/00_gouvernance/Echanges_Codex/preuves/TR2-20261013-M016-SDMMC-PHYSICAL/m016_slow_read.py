from pathlib import Path
import sys,time
P=Path(Path('/tmp/m016-sd-physical-proof').read_text());sys.path.insert(0,str(P));from hardware import *
b=Path('/home/lolo/dev/msm/reports/M016-sdmmc-reference-two-node-build')
debug('attempt02-reset-before-readonly-frequency-check',b/'sdmmc_reference.elf',IDENTITY+'monitor reset\n');time.sleep(3)
debug('readonly-4MHz-differential',b/'sdmmc_reference.elf',IDENTITY+'''p bench_stage
p/x hsd2.CID
if bench_stage != 2 || hsd2.CID[0] != 0x03534453 || hsd2.CID[1] != 0x44333247 || hsd2.CID[2] != 0x858BF092 || hsd2.CID[3] != 0x3C01927A
 quit 4
end
set hsd2.Init.ClockDiv=6
p HAL_SD_ConfigWideBusOperation(&hsd2,0x4000)
p/x hsd2.Instance->CLKCR
p ready()
p transfer(0)
p/x hsd2.ErrorCode
p/x bench_dma_error
p/x bench_last_r1
p/x bench_cmd13_error
p/x hsd2.Instance->CLKCR
'''+ 'dump binary memory '+win(P/'readonly-4MHz-rx.bin')+' rx rx+4096\n',timeout=40)
print((P/'readonly-4MHz-differential.log').read_text(errors='replace'))
