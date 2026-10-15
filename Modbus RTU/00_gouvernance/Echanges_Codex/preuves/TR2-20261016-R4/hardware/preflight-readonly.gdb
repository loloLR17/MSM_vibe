set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
info registers pc sp xpsr
x/2wx 0xE000E100
x/16wx 0x40013000
x/12wx 0x46002000
p hspi1.Init
p SystemCoreClock
p/x *(RCC_TypeDef*)0x46020C00
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4\hardware\flash-before.bin 0x08000000 0x08200000
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4\hardware\flash-verification-read.bin 0x08000000 0x08200000

detach
quit
