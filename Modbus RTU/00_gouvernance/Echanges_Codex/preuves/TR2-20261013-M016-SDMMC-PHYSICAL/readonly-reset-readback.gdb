set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
dump binary memory "\\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-SDMMC-PHYSICAL-20261013T202934Z\readonly-flash-readback.bin" 0x08000000 0x08004e8c
monitor reset

detach
quit
