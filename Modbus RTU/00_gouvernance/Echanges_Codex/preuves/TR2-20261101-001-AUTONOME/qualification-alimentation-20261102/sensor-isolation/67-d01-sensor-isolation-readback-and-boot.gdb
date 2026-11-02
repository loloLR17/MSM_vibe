set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result
p standalone_state
p standalone_fs_readback
p r3_card.commands
p r3_card.reads
p r3_card.writes
p r3_card.initialized
p r3_card.write_enabled
p fifo.running
p r3_retained_available
p sizeof(r3_retained)
p/x *(unsigned int *)0x46002008
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\sensor-isolation\D01-readback\flash.bin 0x08000000 0x800c6e0

detach
quit
