set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
set print elements 24
p standalone_state
p r3_result
if fifo.running || r3_writer.active
 quit 12
end
if standalone_state==1
 watch -l standalone_state
 echo OBSERVING_BOOT_PREFLIGHT_STATE_CHANGE_NO_START\n
 continue
 delete breakpoints
end
p r3_result
p standalone_state
p durable_status
p fifo.running
p r3_writer.active
p r3_writer.uncertain
p r3_writer.needs_power_cycle
p r3_card.writes
p r3_card.error
p r3_card.failed
p r3_clock_fault
p/x *(unsigned int *)0x42021414
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\storage-T10-CN15\red-before-USER\result-after-preflight.bin &r3_result (char *)&r3_result+sizeof(r3_result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\storage-T10-CN15\red-before-USER\status-after-preflight.bin &durable_status (char *)&durable_status+sizeof(durable_status)

detach
quit
