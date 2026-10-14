from pathlib import Path
import sys,json,hashlib
P=Path('/home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014');sys.path.insert(0,str(P));from hardware import *
elf=Path('/home/lolo/dev/msm/reports/SDMMC1-PoC-release-build/sdmmc1_poc.elf')
debug('release-four-bit-card-and-read',elf,IDENTITY+'''p sd_ready
p/x hsd1.CID
p hsd1.SdCard
p/x hsd1.Instance->CLKCR
set $status=(HAL_SD_CardStatusTypeDef *)&_mscd_epbuf.buf
set $s=HAL_SD_GetCardStatus(&hsd1,$status)
p $s
p *$status
set $clk=HAL_RCCEx_GetPeriphCLKFreq((uint64_t)0x00200000)
p $clk
set $cardstate=HAL_SD_GetCardState(&hsd1)
p $cardstate
if sd_ready != 1 || hsd1.SdCard.LogBlockNbr != 62333952 || hsd1.CID[0] != 0x03534453 || hsd1.CID[1] != 0x44333247 || hsd1.CID[2] != 0x858bf092 || hsd1.CID[3] != 0x3C01927A || (hsd1.Instance->CLKCR & 0xc000) != 0x4000 || $status->DataBusWidth != 2 || $s != 0
 quit 4
end
set $read=tud_msc_read10_cb(0,2048,0,&_mscd_epbuf.buf,8192)
p $read
p/x hsd1.ErrorCode
if $read != 8192
 quit 5
end
'''+ 'dump binary memory '+win(P/'release-original-read-8192.bin')+''' &_mscd_epbuf.buf ((unsigned char *)&_mscd_epbuf.buf)+8192
p/x GPIOA->MODER
p/x GPIOA->AFR
x/2wx 0xE000ED28
''',timeout=60)
b=(P/'release-original-read-8192.bin').read_bytes();e=bytes((i*37+(i>>8)+(i>>16)+(i>>24)*113)&255 for i in range(8192));result={'existing_M017_prefix_expected':b==e,'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest(),'access':'original tud_msc_read10_cb; no source modification; no SD write yet'};(P/'release-first-read-verification.json').write_text(json.dumps(result,indent=2)+'\n');print((P/'release-four-bit-card-and-read.log').read_text());print(result)
