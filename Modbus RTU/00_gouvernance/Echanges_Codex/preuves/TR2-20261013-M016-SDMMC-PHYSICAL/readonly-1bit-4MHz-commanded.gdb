set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p/x hsd2.CID
p ready()
set $app=SDMMC_CmdAppCommand(hsd2.Instance,hsd2.SdCard.RelCardAdd<<16)
p/x $app
if $app != 0
 quit 4
end
set $width=SDMMC_CmdBusWidth(hsd2.Instance,0)
p/x $width
if $width != 0
 quit 5
end
set hsd2.Init.BusWide=0
set hsd2.Init.ClockDiv=6
p SDMMC_Init(hsd2.Instance,hsd2.Init)
p/x hsd2.Instance->CLKCR
if (hsd2.Instance->CLKCR & 0xc3ff) != 6
 quit 6
end
p ready()
p transfer(0)
p/x hsd2.ErrorCode
p/x bench_dma_error
p/x bench_last_r1
p/x bench_cmd13_error
p/x hsd2.Instance->CLKCR
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-SDMMC-PHYSICAL-20261013T202934Z\readonly-1bit-4MHz-commanded-rx.bin rx rx+4096

detach
quit
