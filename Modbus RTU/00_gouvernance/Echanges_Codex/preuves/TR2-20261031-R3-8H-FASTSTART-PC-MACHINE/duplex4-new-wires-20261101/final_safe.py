from pathlib import Path
import json,datetime
import hardware_second as h
P=Path(__file__).parent
h.debug('N8E-final-safe-state',P/'arm/r3_coex.elf',h.IDENTITY+"""
p r3_result.stage
p r3_result.error
p r3_result.run
p fifo.running
p r3_writer.active
p r3_clock_fault
p r3_card
p/x *(unsigned int*)0x42021414
p/x *(unsigned int*)0x46002008
if r3_result.stage!=99 || r3_result.error!=1 || r3_result.run!=1 || fifo.running || r3_writer.active || r3_command || r3_clock_fault || r3_card.writes || r3_card.failed || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end
""")
out=dict(prepared=False,status='BLOCKED',utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),source_sha='4307b95cf1f85266266574a04beab96e2fc14b20',reason='FIFO configuration readback 0x06 instead of 0x86; calibration not reached',stage=99,error=1,run=1,fifo_running=False,writer_active=False,SD_writes=0,SD_CS_high=True,clock_fault=0,debugger_detached=True,long_started=False,gate_started=False,firmware_retained='N8E duplex 4MHz, boot WAIT; not qualified')
(P/'READY.json').write_text(json.dumps(out,indent=2)+'\n')
print(json.dumps(out))
