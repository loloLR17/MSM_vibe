from pathlib import Path
import sys
P=Path('/home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014');sys.path.insert(0,str(P));from hardware import *
elf=Path('/home/lolo/dev/msm/reports/SDMMC1-PoC-release-build/sdmmc1_poc.elf')
body=IDENTITY+'''p sd_ready
p/x hsd1.ErrorCode
p/x hsd1.Instance->CLKCR
set $usb=(dwc2_regs_t *)0x42040000
p/x $usb->gotgctl
p/x $usb->gahbcfg
p/x $usb->gusbcfg
p/x $usb->gintsts
p/x $usb->gintmsk
p/x $usb->stm32_gccfg
p/x $usb->dcfg
p/x $usb->dctl
p/x $usb->dsts
set $gpioa=(GPIO_TypeDef *)0x42020000
p/x $gpioa->MODER
p/x $gpioa->AFR[1]
p/x $gpioa->PUPDR
p/x $gpioa->IDR
p _usbd_rhport
p _usbd_dev
x/4wx 0xE000E100
x/2wx 0xE000ED28
'''
debug('usb-native-readonly-state',elf,body,timeout=45);print((P/'usb-native-readonly-state.log').read_text())
