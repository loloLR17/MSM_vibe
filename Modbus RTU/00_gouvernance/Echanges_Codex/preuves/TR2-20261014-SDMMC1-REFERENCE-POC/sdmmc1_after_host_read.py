from pathlib import Path
import sys
P=Path('/home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014');sys.path.insert(0,str(P));from hardware import *
elf=Path('/home/lolo/dev/msm/reports/SDMMC1-PoC-release-build/sdmmc1_poc.elf')
debug('after-host-read-static',elf,IDENTITY+'''p sd_ready
p/x hsd1.ErrorCode
p hsd1.State
p/x hsd1.Instance->STA
p/x hsd1.Instance->RESP1
p/x hsd1.Instance->CLKCR
p hsd1.Instance->DCOUNT
p hsd1.Instance->DLEN
p _usbd_dev.connected
p _usbd_dev.cfg_num
p ejected
x/2wx 0xE000ED28
''');print((P/'after-host-read-static.log').read_bytes().decode(errors='replace'))
