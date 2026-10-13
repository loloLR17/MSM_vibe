from pathlib import Path
import sys,json,hashlib,time,subprocess
P=Path('/home/lolo/dev/msm/reports/TR2-M017-20261014');sys.path.insert(0,str(P));from hardware import *
b=Path('/home/lolo/dev/msm/reports/M017-diagnostic-build');elf=b/'sdmmc_reference.elf';image=b/'sdmmc_reference.bin';sha='65ff1fa93e3793c1ad4e18fe105034bedf1b5146'
a={'code_sha':sha,'bin_bytes':image.stat().st_size,'bin_sha256':hashlib.sha256(image.read_bytes()).hexdigest(),'elf_sha256':hashlib.sha256(elf.read_bytes()).hexdigest(),'ring_bytes':131072,'sdk_version':'v1.9.0'}
(P/'load-artifacts.json').write_text(json.dumps(a,indent=2)+'\n')
debug('load-identity-before-flash',elf,IDENTITY+'x/8wx 0x08000000\n')
flash('load-flash',image,sha)
debug('load-flash-readback',elf,IDENTITY+'dump binary memory '+win(P/'load-flash-readback.bin')+' 0x08000000 '+hex(0x08000000+image.stat().st_size)+'\nmonitor reset\n')
assert image.read_bytes()==(P/'load-flash-readback.bin').read_bytes();time.sleep(2)
debug('load-init-trigger',elf,IDENTITY+'''p load_stage
p/x GPIOB->MODER
if load_stage != 1 || load_trigger != 0 || (GPIOB->MODER & 0xc00003c0) != 0
 quit 4
end
set load_trigger=0x4C31494E
''');time.sleep(2)
debug('load-init-preflight',elf,IDENTITY+'''p load_stage
p load_error
p/x load_cid
p load_blocks
p load_kernel_hz
p load_clock_hz
p load_bus_card
p/x load_clkcr
p/x hsd2.ErrorCode
p SystemCoreClock
if load_stage != 2 || load_error != 0 || load_clock_hz != 10000000 || load_bus_card != 0 || load_generated != 0 || load_written != 0
 quit 4
end
''')
z=subprocess.run(['python3','experiments/sdmmc_st_reference/run_m017.py','--proof',str(P),'--elf',str(elf),'--code-sha',sha,'--run','1']);z.check_returncode()
