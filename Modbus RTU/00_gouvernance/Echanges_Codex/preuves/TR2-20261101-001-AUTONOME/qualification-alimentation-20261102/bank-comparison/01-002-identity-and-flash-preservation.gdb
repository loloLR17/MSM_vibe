set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
info registers pc sp lr xpsr
x/8wx 0x08000000
x/wx 0x46020cf4
x/wx 0x42021414
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\bank-comparison\initial-flash-1.bin 0x08000000 0x08200000
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\bank-comparison\initial-flash-2.bin 0x08000000 0x08200000

detach
quit
