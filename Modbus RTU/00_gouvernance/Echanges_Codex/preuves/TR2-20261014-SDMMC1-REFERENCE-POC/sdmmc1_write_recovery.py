from pathlib import Path
import sys,time,subprocess,json
P=Path('/home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014');sys.path.insert(0,str(P));from hardware import *
elf=Path('/home/lolo/dev/msm/reports/SDMMC1-PoC-release-build/sdmmc1_poc.elf')
debug('before-native-write-recovery',elf,IDENTITY+'''p/x hsd1.ErrorCode
p/x hsd1.CID
p hsd1.SdCard.LogBlockNbr
p/x hsd1.Instance->RESP1
p/x hsd1.Instance->CLKCR
p sd_ready
p _usbd_dev.cfg_num
p ejected
x/2wx 0xE000ED28
monitor reset
''');time.sleep(8)
# Pure register/identity preflight, no SD transfer or parameter alteration.
debug('native-write-recovery-preflight',elf,IDENTITY+'''p sd_ready
p/x hsd1.CID
p hsd1.SdCard.LogBlockNbr
p/x hsd1.ErrorCode
p/x hsd1.Instance->CLKCR
if sd_ready != 1 || hsd1.CID[0] != 0x03534453 || hsd1.CID[1] != 0x44333247 || hsd1.CID[2] != 0x858bf092 || hsd1.CID[3] != 0x3C01927A || hsd1.SdCard.LogBlockNbr != 62333952 || (hsd1.Instance->CLKCR & 0xc000) != 0x4000
 quit 4
end
''')
s=Path('/tmp/sdmmc1_native_aligned.py').read_text().replace('native-aligned-result.json','native-aligned-recovered-result.json').replace('native-aligned-invocation.json','native-aligned-recovered-invocation.json').replace('native-aligned-readback.bin','native-aligned-recovered-readback.bin').replace('native-aligned-verification.json','native-aligned-recovered-verification.json')
(P/'native_aligned_recovered.py').write_text(s)
z=subprocess.run(['python3',str(P/'native_aligned_recovered.py')]);z.check_returncode()
