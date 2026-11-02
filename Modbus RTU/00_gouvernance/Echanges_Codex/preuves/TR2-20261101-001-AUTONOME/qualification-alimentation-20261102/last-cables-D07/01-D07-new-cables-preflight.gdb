set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
set print elements 24
p r3_result
p standalone_state
p standalone_fs_readback
p r3_clock_fault
p fifo.running
p fifo.stopped
p r3_writer.active
p r3_card.commands
p r3_card.reads
p r3_card.writes
p r3_card.initialized
p r3_card.write_enabled
p/x *(unsigned int *)0x42021414
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\last-cables-D07\preflight-flash.bin 0x08000000 0x800c6e0

detach
quit
