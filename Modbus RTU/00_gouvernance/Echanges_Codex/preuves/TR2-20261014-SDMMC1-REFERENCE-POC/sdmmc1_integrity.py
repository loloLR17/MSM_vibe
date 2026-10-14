from pathlib import Path
import sys,hashlib,json
P=Path('/home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014');sys.path.insert(0,str(P));from hardware import *
elf=Path('/home/lolo/dev/msm/reports/SDMMC1-PoC-release-build/sdmmc1_poc.elf')
# Independent host patterns, only shared MSC RAM is restored, no firmware patch.
for j in range(16):
 offset=j*8192;b=bytes((i*73+(i>>8)*13+(i>>16)*19+0x31)&255 for i in range(offset,offset+8192));(P/f'payload-{j:02d}.bin').write_bytes(b)
body=IDENTITY+'''p sd_ready
p/x hsd1.CID
p hsd1.SdCard
p/x hsd1.Instance->CLKCR
if sd_ready != 1 || (hsd1.Instance->CLKCR & 0xc000) != 0x4000 || hsd1.CID[0] != 0x03534453 || hsd1.CID[1] != 0x44333247 || hsd1.CID[2] != 0x858bf092 || hsd1.CID[3] != 0x3C01927A || hsd1.SdCard.LogBlockNbr < 506304
 quit 4
end
'''
for j in range(16):
 lba=506048+j*16
 body+='restore '+win(P/f'payload-{j:02d}.bin')+' binary &_mscd_epbuf.buf\n'
 body+=f'set $w=tud_msc_write10_cb(0,{lba},0,&_mscd_epbuf.buf,8192)\nprintf "WRITE chunk={j} lba={lba} result=%d\\n", $w\np/x hsd1.ErrorCode\nif $w != 8192\n quit 5\nend\n'
# All writes finish before integral stream re-read of the entire region.
for j in range(16):
 lba=506048+j*16
 body+=f'set $r=tud_msc_read10_cb(0,{lba},0,&_mscd_epbuf.buf,8192)\nprintf "READ chunk={j} lba={lba} result=%d\\n", $r\np/x hsd1.ErrorCode\nif $r != 8192\n quit 6\nend\n'
 body+='dump binary memory '+win(P/f'integrity-read-{j:02d}.bin')+' &_mscd_epbuf.buf ((unsigned char *)&_mscd_epbuf.buf)+8192\n'
body+='p/x hsd1.Instance->CLKCR\np/x hsd1.ErrorCode\nset $state=HAL_SD_GetCardState(&hsd1)\np $state\nx/2wx 0xE000ED28\n'
debug('original-callback-integrity-128KiB',elf,body,timeout=180)
checks=[]
for j in range(16):
 b=(P/f'integrity-read-{j:02d}.bin').read_bytes();e=(P/f'payload-{j:02d}.bin').read_bytes();assert b==e
 checks.append({'chunk':j,'lba':506048+j*16,'bytes':8192,'mismatches':0,'sha256_read':hashlib.sha256(b).hexdigest(),'sha256_expected':hashlib.sha256(e).hexdigest()})
(P/'integrity-128KiB.json').write_text(json.dumps({'write_bytes':131072,'read_bytes':131072,'mismatches':0,'first_mismatch':None,'chunks':16,'original_callbacks':True,'no_source_bus_clock_or_HAL_change':True,'method':'GDB calls callbacks with no halt inside transfer; pauses between chunks; not a sustained benchmark','checks':checks},indent=2)+'\n');print('ORIGINAL_CALLBACKS_128KiB_INTEGRAL_MATCH; no benchmark claimed',flush=True)
