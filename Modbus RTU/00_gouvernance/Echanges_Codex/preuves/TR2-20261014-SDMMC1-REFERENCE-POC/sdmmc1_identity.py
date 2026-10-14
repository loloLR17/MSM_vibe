from pathlib import Path
import sys,hashlib,json
P=Path('/home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014');sys.path.insert(0,str(P));from hardware import *
old=Path('/home/lolo/dev/msm/reports/TR2-M017-20261014/hint-image.elf');a=json.loads(Path('/home/lolo/dev/msm/reports/TR2-M017-20261014/hint-artifacts.json').read_text())
debug('identity-and-prior-flash',old,IDENTITY+'dump binary memory '+win(P/'prior-flash-prefix.bin')+' 0x08000000 '+hex(0x08000000+a['bin_bytes'])+'\nx/2wx 0xE000ED28\np load_stage\np/x GPIOB->MODER\n')
matched=hashlib.sha256((P/'prior-flash-prefix.bin').read_bytes()).hexdigest()==a['bin_sha256'];(P/'prior-image-check.json').write_text(json.dumps({'matches_M017_hint_image':matched,'bytes':a['bin_bytes'],'sha256':hashlib.sha256((P/'prior-flash-prefix.bin').read_bytes()).hexdigest(),'no_flash_reset_or_card_command':True},indent=2)+'\n');assert matched
print('SECOND_UID_VERIFIED; prior M017 image matches; no reset/flash/SD access; card absent per user contract')
