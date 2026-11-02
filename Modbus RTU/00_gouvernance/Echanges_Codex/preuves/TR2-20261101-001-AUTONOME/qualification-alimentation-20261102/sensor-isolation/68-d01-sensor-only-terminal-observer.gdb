set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
set print elements 64
p r3_result
p standalone_state
p r3_clock_fault
p r3_card.writes
p/x *(unsigned int *)0x46002008
p/x *(unsigned int *)0x40003808
if r3_result.stage != 2 || r3_result.run != 1 || standalone_state != 2 || r3_result.error || r3_clock_fault || fifo.running || r3_writer.active
 quit 11
end
if r3_card.commands || r3_card.reads || r3_card.writes || r3_card.initialized || r3_card.write_enabled
 quit 12
end
thbreak r3_finished
thbreak Error_Handler
echo OBSERVER_ARMED_WAITING_FOR_USER\n
continue
delete breakpoints
p r3_result
p standalone_state
p r3_retained_available
p/x r3_retained.guard
p r3_retained.payload.result
p r3_retained.payload.writer
p r3_retained.payload.mapper
p r3_retained.payload.last_clock_bracket
p r3_retained.payload.sensor_only
p r3_retained.payload.budget_mask
p r3_retained.payload.budget_record
p r3_retained.payload.maximum_cycles
p r3_retained.payload.queue.head
p r3_retained.payload.queue.tail
p r3_retained.payload.queue.count
p r3_retained.payload.frame_count
p r3_retained.payload.frame_seq
p r3_retained.payload.first_output
p/x r3_retained.payload.source_sha256
p fifo.running
p fifo.stopped
p r3_writer.active
p r3_writer.uncertain
p r3_writer.needs_power_cycle
p/x *(unsigned int *)0x42021414
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\sensor-isolation\D01-terminal\snapshot.bin &r3_retained (char *)&r3_retained+sizeof(r3_retained)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\sensor-isolation\D01-terminal\result.bin &r3_retained.payload.result (char *)&r3_retained.payload.result+sizeof(r3_retained.payload.result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\sensor-isolation\D01-terminal\writer.bin &r3_retained.payload.writer (char *)&r3_retained.payload.writer+sizeof(r3_retained.payload.writer)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\sensor-isolation\D01-terminal\queue.bin &r3_retained.payload.queue (char *)&r3_retained.payload.queue+sizeof(r3_retained.payload.queue)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\sensor-isolation\D01-terminal\source-hash-state.bin &r3_retained.payload.source_hash (char *)&r3_retained.payload.source_hash+sizeof(r3_retained.payload.source_hash)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\sensor-isolation\D01-terminal\source-sha.bin &r3_retained.payload.source_sha256 (char *)&r3_retained.payload.source_sha256+sizeof(r3_retained.payload.source_sha256)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\sensor-isolation\D01-terminal\header.bin &r3_retained.payload.header (char *)&r3_retained.payload.header+sizeof(r3_retained.payload.header)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\sensor-isolation\D01-terminal\partial-frame.bin &r3_retained.payload.partial_frame (char *)&r3_retained.payload.partial_frame+sizeof(r3_retained.payload.partial_frame)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\sensor-isolation\D01-terminal\records.bin &r3_records (char *)&r3_records+sizeof(r3_records)

detach
quit
