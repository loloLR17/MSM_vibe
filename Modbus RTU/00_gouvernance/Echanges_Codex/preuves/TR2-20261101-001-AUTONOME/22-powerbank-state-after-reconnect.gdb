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
p durable_status
p r3_writer
p q.count
p fifo.running
p fifo.stopped
p r3_clock_fault
p r3_card.error
p r3_card.failed
p r3_card.writes
p/x r3_source_sha
p r3_distributions[0].max
p r3_distributions[1].max
p r3_distributions[2].max
p r3_sd_records[0]
bt 5
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\powerbank-a1b-recovery\result.bin &r3_result ((char *)&r3_result)+sizeof(r3_result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\powerbank-a1b-recovery\status.bin &durable_status ((char *)&durable_status)+sizeof(durable_status)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\powerbank-a1b-recovery\source-sha.bin &r3_source_sha ((char *)&r3_source_sha)+sizeof(r3_source_sha)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\powerbank-a1b-recovery\header.bin &r3_header ((char *)&r3_header)+sizeof(r3_header)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\powerbank-a1b-recovery\footer.bin &durable_footer ((char *)&durable_footer)+sizeof(durable_footer)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\powerbank-a1b-recovery\raw-prefix.bin &r3_raw ((char *)&r3_raw)+sizeof(r3_raw)

detach
quit
