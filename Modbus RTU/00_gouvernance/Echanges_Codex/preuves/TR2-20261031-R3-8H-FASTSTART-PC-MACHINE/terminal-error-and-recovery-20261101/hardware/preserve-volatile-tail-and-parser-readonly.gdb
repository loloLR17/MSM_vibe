set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if r3_result.stage!=99 || r3_result.run!=5 || fifo.running || r3_writer.active || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end
p fifo.parser
p fifo.error
p fifo.stop_error
p fifo.running
p fifo.stopped
p r3_spi_fault
p q.head
p q.tail
p q.count
p q.borrowed
p durable_frame_count
p durable_frame_seq
p durable_frame_flags
p durable_first_output
p frame_first_time
p frame_last_time
p durable_status
p r3_writer.uncertain
p r3_card.failed
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-N8G-launch-recovery-20261101T0802Z/long-error-terminal/queue-full.bin &q ((char*)&q)+sizeof(q)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-N8G-launch-recovery-20261101T0802Z/long-error-terminal/partial-frame.bin frame frame+512
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-N8G-launch-recovery-20261101T0802Z/long-error-terminal/fifo-state.bin &fifo ((char*)&fifo)+sizeof(fifo)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-N8G-launch-recovery-20261101T0802Z/long-error-terminal/durable-frame-count.bin &durable_frame_count (&durable_frame_count+1)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-N8G-launch-recovery-20261101T0802Z/long-error-terminal/frame-metadata.bin &frame_first_time ((char*)&frame_first_time)+8

detach
quit
