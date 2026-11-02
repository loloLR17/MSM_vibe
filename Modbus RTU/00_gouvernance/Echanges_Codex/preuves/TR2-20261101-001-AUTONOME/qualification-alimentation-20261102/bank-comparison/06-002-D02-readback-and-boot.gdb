set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\bank-comparison\D02-flash-readback.bin 0x08000000 0x800c6f0
set print elements 32
p r3_result
p standalone_state
p standalone_fs_readback
p r3_card
p fifo.running
p r3_writer.active
p/x *(unsigned int *)0x46002008

detach
quit
