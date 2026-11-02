from pathlib import Path
import sys,json,struct,zlib,hashlib,datetime
base=Path('/home/lolo/dev/msm/reports/TR2-001-autonome-reducteur-20261101');sys.path.insert(0,str(base));import hardware_primary as h
p=base/'qualification-alimentation-20261102/CN0-without-STLINK';d=p/'terminal-recovery';d.mkdir(exist_ok=False);h.P=p/'hardware'
body=h.IDENTITY+'set print elements 32\n'
body+=f'dump binary memory {h.win(d/"snapshot.bin")} &r3_retained (char *)&r3_retained+sizeof(r3_retained)\n'
body+='''info registers pc
p r3_result
p standalone_state
p r3_retained_available
p/x r3_retained.guard
p r3_retained.payload.source_sha
echo RETAINED_RESULT_BEGIN\n
p r3_retained.payload.result
echo RETAINED_RESULT_END\n
p r3_retained.payload.writer
p r3_retained.payload.mapper
p r3_retained.payload.last_clock_bracket
p r3_retained.payload.budget_mask
p r3_retained.payload.maximum_cycles
p r3_retained.payload.queue.head
p r3_retained.payload.queue.tail
p r3_retained.payload.queue.count
p r3_retained.payload.frame_count
p r3_retained.payload.frame_seq
p r3_retained.payload.clock_fault
p fifo.running
p r3_writer.active
p r3_writer.uncertain
p r3_writer.needs_power_cycle
p r3_card.writes
p/x *(unsigned int *)0x42021414
'''
for name,sym in [('result','r3_retained.payload.result'),('writer','r3_retained.payload.writer'),('queue','r3_retained.payload.queue'),('producer-sha','r3_retained.payload.source_sha256'),('header','r3_retained.payload.header'),('partial-frame','r3_retained.payload.partial_frame')]:body+=f'dump binary memory {h.win(d/(name+".bin"))} &{sym} (char *)&{sym}+sizeof({sym})\n'
h.debug('04-CN0-preserve-terminal-before-any-intervention',p/'image/r3_coex.elf',body,timeout=60)
b=(d/'snapshot.bin').read_bytes();magic,version,n,crc=struct.unpack('<IIII',b[:16]);valid=magic==0x31525452 and version==1 and n==len(b)-16 and zlib.crc32(b[16:])==crc
r={'UTC':datetime.datetime.now(datetime.timezone.utc).isoformat(),'valid':valid,'snapshot_SHA256':hashlib.sha256(b).hexdigest(),'source_SHA':b[16:56].decode(errors='replace') if valid else None,'producer_SHA256':(d/'producer-sha.bin').read_bytes().hex() if valid else None,'no_reset_flash_START_SD_command':True,'GDB_detached':True};(d/'validation.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps(r,indent=2));assert valid
