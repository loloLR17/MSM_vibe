set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p/x *(unsigned int *)0x46020cf4
p/x *(unsigned int *)0xE000ED28
p/x *(unsigned int *)0xE000ED2C
p/x *(unsigned int *)0xE000EDF0
p/x *(unsigned int *)0x42021800
p/x *(unsigned int *)0x42021810
p/x *(unsigned int *)0x42021814
p r3_irqs
p r3_spi_fault
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\sd2mhz-a1e-reset-diagnostic\flash.bin 0x08000000 0x0800cce0
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\sd2mhz-a1e-reset-diagnostic\result.bin &r3_result (char *)&r3_result+sizeof(r3_result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\sd2mhz-a1e-reset-diagnostic\card.bin &r3_card (char *)&r3_card+sizeof(r3_card)

detach
quit
