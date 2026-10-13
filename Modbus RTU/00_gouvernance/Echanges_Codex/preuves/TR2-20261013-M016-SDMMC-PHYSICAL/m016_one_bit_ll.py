from pathlib import Path
import sys
P=Path(Path('/tmp/m016-sd-physical-proof').read_text());sys.path.insert(0,str(P));from hardware import *
b=Path('/home/lolo/dev/msm/reports/M016-sdmmc-reference-two-node-build')
debug('readonly-1bit-4MHz-commanded',b/'sdmmc_reference.elf',IDENTITY+'''p/x hsd2.CID
p ready()
set $app=SDMMC_CmdAppCommand(hsd2.Instance,hsd2.SdCard.RelCardAdd<<16)
p/x $app
if $app != 0
 quit 4
end
set $width=SDMMC_CmdBusWidth(hsd2.Instance,0)
p/x $width
if $width != 0
 quit 5
end
set hsd2.Init.BusWide=0
set hsd2.Init.ClockDiv=6
p SDMMC_Init(hsd2.Instance,hsd2.Init)
p/x hsd2.Instance->CLKCR
if (hsd2.Instance->CLKCR & 0xc3ff) != 6
 quit 6
end
p ready()
p transfer(0)
p/x hsd2.ErrorCode
p/x bench_dma_error
p/x bench_last_r1
p/x bench_cmd13_error
p/x hsd2.Instance->CLKCR
'''+ 'dump binary memory '+win(P/'readonly-1bit-4MHz-commanded-rx.bin')+' rx rx+4096\n',timeout=40)
print((P/'readonly-1bit-4MHz-commanded.log').read_text(errors='replace'))
