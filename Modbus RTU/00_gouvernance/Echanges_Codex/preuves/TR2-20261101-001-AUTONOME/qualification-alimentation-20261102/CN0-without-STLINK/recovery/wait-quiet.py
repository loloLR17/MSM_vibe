from pathlib import Path
import sys
base=Path('/home/lolo/dev/msm/reports/TR2-001-autonome-reducteur-20261101');sys.path.insert(0,str(base));import hardware_primary as h
p=base/'qualification-alimentation-20261102/CN0-without-STLINK';h.P=p/'hardware'
body=h.IDENTITY+'''set print elements 16
p r3_result
p fifo.running
p r3_writer.active
p r3_card.writes
if fifo.running || r3_writer.active || r3_card.writes
 quit 12
end
if standalone_state==1
 thbreak Error_Handler
 thbreak main.c:440
 continue
 delete breakpoints
end
p r3_result
p standalone_state
p fifo.running
p r3_writer.active
p r3_writer.uncertain
p r3_writer.needs_power_cycle
p r3_card.writes
p/x *(unsigned int *)0x42021414
p/x r3_retained.guard
'''
h.debug('05-CN0-wait-post-reconnection-readonly-boot-quiescent',p/'image/r3_coex.elf',body,timeout=600)
