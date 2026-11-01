from pathlib import Path
import json
import hardware_second as h
P=Path(__file__).parent;d=P/'long-error-terminal'
body=h.IDENTITY+'''if r3_result.stage!=99 || r3_result.run!=5 || fifo.running || r3_writer.active || !(*(unsigned int*)0x42021414 &0x1000)
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
'''
for name,start,end in [('queue-full','&q','((char*)&q)+sizeof(q)'),('partial-frame','frame','frame+512'),('fifo-state','&fifo','((char*)&fifo)+sizeof(fifo)'),('durable-frame-count','&durable_frame_count','(&durable_frame_count+1)'),('frame-metadata','&frame_first_time','((char*)&frame_first_time)+8')]:
 f=d/(name+'.bin');body+=f'dump binary memory {h.win(f).replace(chr(92),"/")} {start} {end}\n'
h.debug('preserve-volatile-tail-and-parser-readonly',P/'arm/r3_coex.elf',body)
status=json.loads((d/'STATUS.json').read_text());status.pop('FIFO_stopped',None);status.update(FIFO_running=False,FIFO_stop_verification_failed=True,stop_error=6,physical_sensor_ODR_off_not_verified=True,SD_writer_inactive=True,SD_writer_uncertain=False,SD_CS_high=True,volatile_queue_and_partial_frame_preserved=True);(d/'STATUS.json').write_text(json.dumps(status,indent=2)+'\n');print((P/'hardware/preserve-volatile-tail-and-parser-readonly.log').read_text())
