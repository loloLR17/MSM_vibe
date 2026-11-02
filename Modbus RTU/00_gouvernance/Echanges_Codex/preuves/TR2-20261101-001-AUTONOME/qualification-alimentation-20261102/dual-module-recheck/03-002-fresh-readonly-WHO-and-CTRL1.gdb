set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if !fifo.initialized || fifo.running || r3_writer.active || r3_writer.uncertain || r3_writer.needs_power_cycle || r3_card.commands || r3_card.reads || r3_card.writes || r3_card.initialized || r3_card.write_enabled || bench_trace_count>1000 || bench_payload_count>65000
 quit 12
end
set $saved=*(unsigned int *)(r3_header+300)
set $trace_before=bench_trace_count
set $payload_before=bench_payload_count
set $who_rc=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char *)(r3_header+300))
printf "CURRENT_WHO_STATUS=%d CURRENT_WHO=0x%02x\n",$who_rc,*(unsigned char *)(r3_header+300)
set $ctrl_rc=(int)iis3dwb_read_reg(&fifo.ctx,0x10,(unsigned char *)(r3_header+301),1)
printf "CURRENT_CTRL_STATUS=%d CURRENT_CTRL1=0x%02x\n",$ctrl_rc,*(unsigned char *)(r3_header+301)
set *(unsigned int *)(r3_header+300)=$saved
p bench_trace_count
p bench_payload_count
p r3_result.stage
p r3_result.error
p fifo.running
p r3_writer.active
p r3_card.commands
p r3_card.writes
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\dual-module-recheck\002-new-traces.bin &bench_trace[$trace_before] &bench_trace[bench_trace_count]
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\dual-module-recheck\002-new-payload.bin &bench_payload[$payload_before] &bench_payload[bench_payload_count]

detach
quit
