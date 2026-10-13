from pathlib import Path
import sys,time
P=Path(Path('/tmp/m016-sd-physical-proof').read_text());sys.path.insert(0,str(P));from hardware import *
b=Path('/home/lolo/dev/msm/reports/M016-sdmmc-reference-two-node-build')
debug('rising-comparison-reset',b/'sdmmc_reference.elf',IDENTITY+'monitor reset\n');time.sleep(3)
debug('readonly-4bit-4MHz-rising-clean',b/'sdmmc_reference.elf',IDENTITY+'''p bench_stage
p/x hsd2.CID
p/x hsd2.ErrorCode
p/x hsd2.Instance->CLKCR
if bench_stage != 2 || hsd2.CID[0] != 0x03534453 || hsd2.CID[1] != 0x44333247 || hsd2.CID[2] != 0x858BF092 || hsd2.CID[3] != 0x3C01927A
 quit 4
end
set hsd2.Init.ClockDiv=6
set hsd2.Init.ClockEdge=0
p SDMMC_Init(hsd2.Instance,hsd2.Init)
p/x hsd2.Instance->CLKCR
if (hsd2.Instance->CLKCR & 0x1c3ff) != 0x4006
 quit 5
end
p ready()
p transfer(0)
p/x hsd2.ErrorCode
p/x bench_dma_error
p/x bench_last_r1
p/x bench_cmd13_error
p/x hsd2.Instance->CLKCR
'''+ 'dump binary memory '+win(P/'readonly-4bit-4MHz-rising-clean-rx.bin')+' rx rx+4096\n',timeout=40)
print((P/'readonly-4bit-4MHz-rising-clean.log').read_text(errors='replace'))
