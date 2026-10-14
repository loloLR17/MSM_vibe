from pathlib import Path
import sys,time,json,hashlib,subprocess
P=Path('/home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014');sys.path.insert(0,str(P));from hardware import *
b=Path('/home/lolo/dev/msm/reports/SDMMC1-PoC-build');elf=b/'sdmmc1_poc.elf';image=b/'sdmmc1_poc.bin';ref='1e3c9183bad023ebf7d234134823d9270a0e8b64'
a={'third_party_sha':ref,'bin_bytes':image.stat().st_size,'bin_sha256':hashlib.sha256(image.read_bytes()).hexdigest(),'elf_sha256':hashlib.sha256(elf.read_bytes()).hexdigest(),'source_diff':'none; external clean checkout; only Linux build wrapper','compiler':subprocess.check_output(['arm-none-eabi-gcc','--version'],text=True).splitlines()[0]}
(P/'artifacts.json').write_text(json.dumps(a,indent=2)+'\n')
debug('identity-before-flash',elf,IDENTITY+'x/8wx 0x08000000\n')
flash('community-original-flash',image,ref)
debug('community-flash-readback',elf,IDENTITY+'dump binary memory '+win(P/'community-flash-readback.bin')+' 0x08000000 '+hex(0x08000000+image.stat().st_size)+'\n')
assert image.read_bytes()==(P/'community-flash-readback.bin').read_bytes()
# Qualified software reset clears programmer interaction. Capture original calls,
# no HAL parameter is changed and no breakpoint remains within transfers.
debug('community-init-observation',elf,IDENTITY+'''hbreak HAL_SD_Init
monitor reset
continue
delete breakpoints
finish
printf "HAL_SD_Init return=%u\\n", $r0
p/x hsd1.ErrorCode
p/x hsd1.CID
p hsd1.SdCard
if $r0 != 0
 quit 5
end
hbreak HAL_SD_ConfigWideBusOperation
continue
delete breakpoints
finish
printf "HAL_SD_ConfigWideBusOperation return=%u\\n", $r0
p/x hsd1.ErrorCode
p/x hsd1.Instance->CLKCR
p hsd1.Init
p SystemCoreClock
p/x GPIOC->MODER
p/x GPIOC->AFR
p/x GPIOD->MODER
p/x GPIOD->AFR
x/2wx 0xE000ED28
''',timeout=90)
print((P/'community-init-observation.log').read_text())
