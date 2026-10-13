from pathlib import Path
import sys,time,json,hashlib
P=Path(Path('/tmp/m016-gpio-proof').read_text());sys.path.insert(0,str(P));from hardware import *
b=Path('/home/lolo/dev/msm/reports/M016-gpio-dat123-build');im=b/'gpio_dat123.bin';elf=b/'gpio_dat123.elf';code='9b0ad67e8a981226d12fc48addf45ce35489e225'
(P/'artifacts-pwr-fixed.json').write_text(json.dumps({'code_sha':code,'files':{ext:{'sha256':hashlib.sha256((b/('gpio_dat123.'+ext)).read_bytes()).hexdigest(),'bytes':(b/('gpio_dat123.'+ext)).stat().st_size} for ext in ['bin','elf','map']}},indent=2)+'\n')
assert json.loads((P/'physical-safety-confirmation.json').read_text())['user']=='microSD retirée, lignes libres, banc remis sous tension'
debug('gpio-pwr-fixed-identity',elf,IDENTITY+'x/8wx 0x08000000\n')
flash('gpio-pwr-fixed-flash',im,code)
debug('gpio-pwr-fixed-readback-reset',elf,IDENTITY+'dump binary memory '+win(P/'gpio-pwr-fixed-flash-readback.bin')+f' 0x08000000 0x{0x08000000+im.stat().st_size:08X}\nmonitor reset\n');assert (P/'gpio-pwr-fixed-flash-readback.bin').read_bytes()==im.read_bytes();time.sleep(2)
debug('gpio-pwr-fixed-phase0-baseline',elf,IDENTITY+'''p gpio_stage
p gpio_phase
p gpio_command
p/x gpio_output_mask
p gpio_error
p/x GPIOB->MODER
p/x GPIOB->OTYPER
p/x GPIOB->ODR
p/x GPIOB->IDR
p/x GPIOB->PUPDR
p/x RCC->AHB2ENR1
p/x PWR->UCPDR
p/x DBGMCU->CR
x/2wx 0xE000ED28
if gpio_stage != 2 || gpio_phase != 0 || (GPIOB->ODR & 0x8018) != 0 || (RCC->AHB2ENR1 & 0x18000000) != 0
 quit 4
end
''')
print((P/'gpio-phase0-baseline.log').read_text(errors='replace'))
