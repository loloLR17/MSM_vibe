from pathlib import Path
import sys,time,json,hashlib
P=Path('/home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014');sys.path.insert(0,str(P));from hardware import *
elf=Path('/home/lolo/dev/msm/reports/SDMMC1-PoC-build/sdmmc1_poc.elf')
debug('post-crc-card-state',elf,IDENTITY+'''p/x hsd1.ErrorCode
set $state=HAL_SD_GetCardState(&hsd1)
p $state
p/x hsd1.Instance->RESP1
p/x hsd1.Instance->STA
''')
# Reset is justified recovery: original HAL exits DCRC branch without CMD12
# on the incomplete multi-block transfer. No bus/clock/driver parameter change.
debug('same-image-recovery-reset',elf,IDENTITY+'monitor reset\n');time.sleep(2)
debug('same-profile-single-sector',elf,IDENTITY+'''p sd_ready
p/x hsd1.ErrorCode
p/x hsd1.CID
p/x hsd1.Instance->CLKCR
if sd_ready != 1 || (hsd1.Instance->CLKCR & 0xc000) != 0x4000
 quit 4
end
set $r=tud_msc_read10_cb(0,2048,0,&_mscd_epbuf.buf,512)
p $r
p/x hsd1.ErrorCode
p hsd1.Instance->DCOUNT
p/x hsd1.Instance->STA
x/2wx 0xE000ED28
'''+ 'dump binary memory '+win(P/'short-read-attempt.bin')+' &_mscd_epbuf.buf ((unsigned char *)&_mscd_epbuf.buf)+512\n')
print((P/'post-crc-card-state.log').read_text());print((P/'same-profile-single-sector.log').read_text())
