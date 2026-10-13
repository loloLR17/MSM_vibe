from pathlib import Path
import sys,time,json,datetime
P=Path(Path('/tmp/m016-gpio-proof').read_text());sys.path.insert(0,str(P));from hardware import *
phase=int(sys.argv[1]);previous=phase-1;elf=Path('/home/lolo/dev/msm/reports/M016-gpio-dat123-build/gpio_dat123.elf');expected={0:0,1:0x8000,2:0,3:8,4:0,5:16,6:0}[phase]
debug('gpio-phase'+str(phase)+'-command',elf,IDENTITY+f'''p gpio_stage
p gpio_phase
if gpio_stage != 2 || gpio_phase != {previous} || gpio_command != 0
 quit 4
end
set gpio_command=0x{0x47310000|phase:08X}
''');time.sleep(21)
debug('gpio-phase'+str(phase)+'-observed',elf,IDENTITY+f'''p gpio_stage
p gpio_phase
p gpio_command
p/x gpio_output_mask
p gpio_error
p uwTick
p gpio_phase_started_ms
p/x GPIOB->ODR
p/x GPIOB->IDR
p/x GPIOB->MODER
p/x GPIOB->OTYPER
p/x RCC->AHB2ENR1
if gpio_phase != {phase} || (GPIOB->ODR & 0x8018) != {expected} || (GPIOB->IDR & 0x8018) != {expected} || (RCC->AHB2ENR1 & 0x18000000) != 0
 quit 5
end
''')
(P/('phase'+str(phase)+'-announced.json')).write_text(json.dumps({'phase':phase,'expected_gpio_mask':hex(expected),'observed_verified':True,'at_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'max_high_dwell_HAL_ms':300000},indent=2)+'\n')
print((P/('gpio-phase'+str(phase)+'-observed.log')).read_text(errors='replace'))
