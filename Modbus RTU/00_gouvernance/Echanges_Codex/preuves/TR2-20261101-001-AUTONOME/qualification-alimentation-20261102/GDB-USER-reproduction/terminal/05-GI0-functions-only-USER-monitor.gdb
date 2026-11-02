set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
set print elements 32
p r3_result
p standalone_state
if r3_result.stage==2 && r3_result.run==1 && standalone_state==2 && !r3_result.error && !fifo.running && !r3_writer.active
 thbreak Error_Handler
 thbreak r3_finished
 thbreak durable_prepare
 commands
  silent
  echo USER_RECOGNIZED_PREPARE_ENTER_FIFO_OFF\n
  p r3_result.stage
  p r3_result.run
  p bytes
  p r3_clock_fault
  continue
 end
 thbreak fifo_live_start
 commands
  silent
  echo PHASE_FIFO_START_ATTEMPT\n
  p r3_clock_fault
  p durable_status
  continue
 end
 echo READY_FOR_USER_NO_START_ISSUED\n
 continue
end
delete breakpoints
echo TERMINAL_OR_STARTUP_REFUSAL_CAPTURE\n
info registers pc
p r3_result
p standalone_state
p r3_retained_available
p/x r3_retained.guard
p fifo
p r3_writer
p durable_status
p r3_card
p mapper
p q.head
p q.tail
p q.count
p durable_frame_count
p durable_frame_seq
p durable_first_output
p r3_clock_fault
p bench_trace_count
p bench_payload_count
p bench_preclose_count
p/x *(unsigned int *)0x42021414
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\GDB-USER-reproduction\observed-terminal-functions\snapshot.bin &r3_retained (char *)&r3_retained+sizeof(r3_retained)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\GDB-USER-reproduction\observed-terminal-functions\result.bin &r3_result (char *)&r3_result+sizeof(r3_result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\GDB-USER-reproduction\observed-terminal-functions\writer.bin &r3_writer (char *)&r3_writer+sizeof(r3_writer)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\GDB-USER-reproduction\observed-terminal-functions\queue.bin &q (char *)&q+sizeof(q)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\GDB-USER-reproduction\observed-terminal-functions\source-sha.bin &r3_source_sha (char *)&r3_source_sha+sizeof(r3_source_sha)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\GDB-USER-reproduction\observed-terminal-functions\header.bin &r3_header (char *)&r3_header+sizeof(r3_header)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\GDB-USER-reproduction\observed-terminal-functions\partial-frame.bin &frame (char *)&frame+sizeof(frame)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\GDB-USER-reproduction\observed-terminal-functions\card.bin &r3_card (char *)&r3_card+sizeof(r3_card)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\GDB-USER-reproduction\observed-terminal-functions\status.bin &durable_status (char *)&durable_status+sizeof(durable_status)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\GDB-USER-reproduction\observed-terminal-functions\fifo.bin &fifo (char *)&fifo+sizeof(fifo)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\GDB-USER-reproduction\observed-terminal-functions\trace.bin &bench_trace (char *)&bench_trace+sizeof(bench_trace)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\GDB-USER-reproduction\observed-terminal-functions\payload.bin &bench_payload (char *)&bench_payload+sizeof(bench_payload)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\GDB-USER-reproduction\observed-terminal-functions\preclose.bin &bench_preclose (char *)&bench_preclose+sizeof(bench_preclose)

detach
quit
