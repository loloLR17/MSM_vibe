set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p sd_ready
p/x hsd1.CID
p hsd1.SdCard.LogBlockNbr
p hsd1.SdCard.LogBlockSize
p/x hsd1.ErrorCode
p hsd1.State
p/x hsd1.Instance->CLKCR
p/x hsd1.Instance->RESP1
p/x hsd1.Instance->STA
p SystemCoreClock
p/x ((RCC_TypeDef *)0x46020c00)->CR
p/x ((RCC_TypeDef *)0x46020c00)->CCIPR1
p/x ((RCC_TypeDef *)0x46020c00)->CCIPR2
p _usbd_dev.connected
p _usbd_dev.cfg_num
p ejected
x/2wx 0xE000ED28
if sd_ready != 1 || hsd1.CID[0] != 0x03534453 || hsd1.CID[1] != 0x44333247 || hsd1.CID[2] != 0x858bf092 || hsd1.CID[3] != 0x3C01927A || hsd1.SdCard.LogBlockNbr != 62333952 || hsd1.SdCard.LogBlockSize != 512 || hsd1.Instance->CLKCR != 0x4002 || ( ((RCC_TypeDef *)0x46020c00)->CCIPR1 & 0x0c000000) != 0 || ( ((RCC_TypeDef *)0x46020c00)->CCIPR2 & 0x4000) != 0 || (((RCC_TypeDef *)0x46020c00)->CR & 0x3000) != 0x3000
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-SDMMC1-COLDSTART10CM-20261014\initial-flash-readback.bin 0x08000000 0x8007acd

detach
quit
