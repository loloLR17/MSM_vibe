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
p durable_status
p fs
p r3_card.error
p r3_card.reads
p r3_card.writes
p r3_writer
p q.count
p fifo.running
p fifo.stopped
p r3_clock_fault
p/x r3_source_sha
p *(GPIO_TypeDef *)0x42021400
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\gate-a1a-results\result.bin &r3_result ((char *)&r3_result)+sizeof(r3_result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\gate-a1a-results\status.bin &durable_status ((char *)&durable_status)+sizeof(durable_status)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\gate-a1a-results\source-sha.bin &r3_source_sha ((char *)&r3_source_sha)+sizeof(r3_source_sha)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\gate-a1a-results\mbr.bin &tr2_001_mbr ((char *)&tr2_001_mbr)+sizeof(tr2_001_mbr)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\gate-a1a-results\bpb.bin &tr2_001_boot ((char *)&tr2_001_boot)+sizeof(tr2_001_boot)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\gate-a1a-results\header.bin &r3_header ((char *)&r3_header)+sizeof(r3_header)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\gate-a1a-results\footer.bin &durable_footer ((char *)&durable_footer)+sizeof(durable_footer)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\gate-a1a-results\flash-readback.bin 0x08000000 0x800ccc4

detach
quit
