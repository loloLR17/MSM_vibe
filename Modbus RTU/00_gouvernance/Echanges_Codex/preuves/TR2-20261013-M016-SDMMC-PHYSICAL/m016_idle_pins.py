from pathlib import Path
import sys
P=Path(Path('/tmp/m016-sd-physical-proof').read_text());sys.path.insert(0,str(P));from hardware import *
b=Path('/home/lolo/dev/msm/reports/M016-sdmmc-reference-two-node-build')
debug('final-readonly-pin-state',b/'sdmmc_reference.elf',IDENTITY+'''p bench_stage
p bench_trigger
p/x hsd2.Instance->CLKCR
p/x GPIOB->IDR
p/x GPIOB->MODER
p/x GPIOB->AFR
p/x GPIOB->PUPDR
p/x DBGMCU->CR
p/x PWR->UCPDR
p/x hsd2.ErrorCode
x/2wx 0xE000ED28
''')
print((P/'final-readonly-pin-state.log').read_text(errors='replace'))
