from pathlib import Path
import sys,time,hashlib,json
P=Path(Path('/tmp/m016-load-proof').read_text());sys.path.insert(0,str(P));from hardware import *
b=Path('/home/lolo/dev/msm/reports/M016-1bit-load-build');elf=b/'sdmmc_reference.elf';image=b/'sdmmc_reference.bin';a=json.loads((P/'artifacts.json').read_text());assert hashlib.sha256(image.read_bytes()).hexdigest()==a['files']['bin']['sha256']
debug('load-second-identity',elf,IDENTITY+'x/8wx 0x08000000\n')
flash('load-safe-flash',image,a['code_sha'])
debug('load-flash-readback-reset',elf,IDENTITY+'dump binary memory '+win(P/'load-flash-readback.bin')+f' 0x08000000 0x{0x08000000+image.stat().st_size:08X}\nmonitor reset\n');assert (P/'load-flash-readback.bin').read_bytes()==image.read_bytes();time.sleep(2)
debug('load-safe-no-card-state',elf,IDENTITY+'''p load_stage
p load_trigger
p load_error
p/x GPIOB->MODER
p/x GPIOB->OTYPER
p/x hsd2.Instance->POWER
p/x hsd2.Instance->CLKCR
p hsd2.Init
p SystemCoreClock
p/x DWT->CTRL
p DWT->CYCCNT
x/2wx 0xE000ED28
if load_stage != 1 || load_trigger != 0 || load_error != 0 || (GPIOB->MODER & 0xc00003c0) != 0 || hsd2.Instance->POWER != 0
 quit 4
end
''');print((P/'load-safe-no-card-state.log').read_text(errors='replace'))
