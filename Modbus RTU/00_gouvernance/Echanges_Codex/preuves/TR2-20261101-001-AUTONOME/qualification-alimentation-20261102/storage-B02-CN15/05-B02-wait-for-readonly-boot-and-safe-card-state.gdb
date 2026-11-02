set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p standalone_state
p r3_result.stage
p r3_result.error
p fifo.running
p r3_writer.active
if fifo.running || r3_writer.active
 quit 12
end
if standalone_state==1
 thbreak Error_Handler
 echo WAITING_READONLY_BOOT_PREFLIGHT_TO_FINISH\n
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
p r3_card.last_cmd
p r3_card.failed
p/x *(unsigned int *)0x42021414

detach
quit
