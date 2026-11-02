from pathlib import Path
import sys,json,hashlib,datetime
base=Path('/home/lolo/dev/msm/reports/TR2-001-autonome-reducteur-20261101');sys.path.insert(0,str(base));import hardware_primary as h
q=base/'qualification-alimentation-20261102/CN0-without-STLINK';h.P=q/'hardware';image=q/'image/r3_coex.bin';meta=json.loads((q/'image.json').read_text());assert image.stat().st_size==53340 and hashlib.sha256(image.read_bytes()).hexdigest()==meta['SHA256'];assert meta['source_SHA']=='6633b0154bf67b4c5437f1d259732fc1b1f21e4a'
previous=base/'qualification-alimentation-20261102/GDB-USER-reproduction/image/r3_coex.elf'
body=h.IDENTITY+'''p r3_result
p standalone_state
p fifo.running
p r3_writer.active
p r3_writer.uncertain
p r3_writer.needs_power_cycle
if r3_result.stage==3 || fifo.running || r3_writer.active || r3_writer.uncertain || r3_writer.needs_power_cycle
 quit 12
end
'''
h.debug('01-CN0-target-safe-before-programming',previous,body)
h.flash('02-CN0-standard-programming',image,meta['source_SHA'])
h.debug('03-CN0-boot-with-reader-empty',q/'image/r3_coex.elf',h.IDENTITY+'''p r3_result
p standalone_state
p fifo.running
p r3_writer.active
p r3_card.writes
p/x *(unsigned int*)0x42021414
''')
(q/'loaded.json').write_text(json.dumps({'UTC':datetime.datetime.now(datetime.timezone.utc).isoformat(),'target_SN':h.SN,'BIN_SHA256':meta['SHA256'],'source_SHA':meta['source_SHA'],'standard_programming_verified':True,'no_SD_during_flash':True,'no_START':True,'GDB_detached':True},indent=2)+'\n');print('CN0 loaded, verified; empty-reader boot observed; GDB detached')
