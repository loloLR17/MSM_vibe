set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p/x hsd2.Instance->CLKCR
p/x GPIOB->MODER
p/x GPIOB->AFR
p/x GPIOD->MODER
p/x GPIOD->AFR
p HAL_SD_DeInit(&hsd2)
set hsd2.Init.BusWide=0
set hsd2.Init.ClockDiv=6
p HAL_SD_Init(&hsd2)
p/x hsd2.CID
p/x hsd2.Instance->CLKCR
p/x hsd2.ErrorCode
if hsd2.CID[0] != 0x03534453 || hsd2.CID[1] != 0x44333247 || hsd2.CID[2] != 0x858BF092 || hsd2.CID[3] != 0x3C01927A || (hsd2.Instance->CLKCR & 0xc3ff) != 6
 quit 4
end
p ready()
p transfer(0)
p/x hsd2.ErrorCode
p/x bench_dma_error
p/x bench_last_r1
p/x bench_cmd13_error
p/x hsd2.Instance->CLKCR
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-SDMMC-PHYSICAL-20261013T202934Z\readonly-1bit-4MHz-rx.bin rx rx+4096

detach
quit
