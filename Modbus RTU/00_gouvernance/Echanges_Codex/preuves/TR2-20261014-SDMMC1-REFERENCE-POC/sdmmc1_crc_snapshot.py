from pathlib import Path
import sys
P=Path('/home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014');sys.path.insert(0,str(P));from hardware import *
elf=Path('/home/lolo/dev/msm/reports/SDMMC1-PoC-build/sdmmc1_poc.elf')
debug('first-crc-static-snapshot',elf,IDENTITY+'''p/x hsd1.ErrorCode
p hsd1.State
p sd_ready
p/x hsd1.Instance->STA
p/x hsd1.Instance->CMD
p/x hsd1.Instance->RESP1
p/x hsd1.Instance->CLKCR
p/x hsd1.Instance->POWER
p/x hsd1.Instance->DCTRL
p hsd1.Instance->DCOUNT
p hsd1.Instance->DLEN
p/x GPIOC->MODER
p/x GPIOC->PUPDR
p/x GPIOC->OSPEEDR
p/x GPIOC->AFR
p/x GPIOC->IDR
p/x GPIOD->MODER
p/x GPIOD->PUPDR
p/x GPIOD->AFR
p/x GPIOD->IDR
p/x GPIOA->MODER
p/x GPIOA->AFR
set $kernel=HAL_RCCEx_GetPeriphCLKFreq((uint64_t)0x00200000)
p $kernel
p SystemCoreClock
p HAL_GetTick()
x/2wx 0xE000ED28
''')
print((P/'first-crc-static-snapshot.log').read_text())
