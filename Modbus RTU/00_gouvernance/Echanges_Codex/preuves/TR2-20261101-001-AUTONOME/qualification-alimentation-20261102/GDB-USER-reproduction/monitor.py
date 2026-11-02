from pathlib import Path
import sys,json,struct,zlib,hashlib,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-001-autonome-reducteur-20261101');sys.path.insert(0,str(p));import hardware_primary as h
q=Path(Path('/tmp/tr2_001_monitor_path.txt').read_text());h.P=q/'hardware';d=q/'observed-terminal';d.mkdir(exist_ok=True);a=q/'image'
body=h.IDENTITY+r'''set print elements 32
p r3_result
p standalone_state
p fifo.running
p r3_writer.active
if standalone_state<98
 thbreak Error_Handler
 thbreak r3_finished
 thbreak durable_prepare
 condition $bpnum run==5
 commands
  silent
  echo PHASE_PREPARE_ENTER_FIFO_OFF\n
  p r3_result.stage
  p r3_result.run
  p bytes
  p r3_clock_fault
  continue
 end
 thbreak fifo_live_start
 condition $bpnum run==5
 commands
  silent
  echo PHASE_FIFO_START_ATTEMPT\n
  p r3_clock_fault
  p durable_status
  continue
 end
 watch -l standalone_state
 commands
  silent
  printf "STATE_CHANGE=%u\n",standalone_state
  if standalone_state==2
   echo READY_FOR_USER_NO_START_ISSUED\n
   p r3_result
  end
  if standalone_state==3
   echo USER_RECOGNIZED_BEFORE_PREPARATION\n
  end
  continue
 end
 echo MONITOR_ARMED_NO_START_ISSUED\n
 if standalone_state==2
  echo READY_FOR_USER_NO_START_ISSUED\n
 end
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
'''
for name,sym in [('snapshot','r3_retained'),('result','r3_result'),('writer','r3_writer'),('queue','q'),('source-sha','r3_source_sha'),('header','r3_header'),('partial-frame','frame'),('card','r3_card'),('status','durable_status'),('fifo','fifo'),('trace','bench_trace'),('payload','bench_payload'),('preclose','bench_preclose')]:body+=f'dump binary memory {h.win(d/(name+".bin"))} &{sym} (char *)&{sym}+sizeof({sym})\n'
(q/'monitor-started.json').write_text(json.dumps({'UTC':datetime.datetime.now(datetime.timezone.utc).isoformat(),'target_SN':h.SN,'no_agent_START':True,'no_reset_flash_in_monitor':True,'breaks':'state writes before FIFO/afterSTOP, prepare entry, FIFO start entry, Error_Handler/r3_finished; never periodic halt during active FIFO','timeout_s':10800},indent=2)+'\n')
h.debug('04-GI0-persistent-USER-monitor',a/'r3_coex.elf',body,timeout=10800)
b=(d/'snapshot.bin').read_bytes();magic,ver,n,crc=struct.unpack('<IIII',b[:16]);valid=magic==0x31525452 and ver==1 and n==len(b)-16 and zlib.crc32(b[16:])==crc;(d/'snapshot-validation.json').write_text(json.dumps({'valid':valid,'SHA256':hashlib.sha256(b).hexdigest(),'source':b[16:56].decode(errors='replace') if valid else None},indent=2)+'\n');print('GI0 observation terminal preserved, GDB detached')
