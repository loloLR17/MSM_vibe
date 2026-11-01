from pathlib import Path
import json,ast,hashlib,subprocess,datetime
import hardware_second as h
import run_capture as c
P=Path(__file__).parent;d=P/'long-error-terminal';d.mkdir(exist_ok=False)
a=json.loads((P/'n8g-long-launched.json').read_text());native=Path(subprocess.check_output(['wslpath','-u',a['native_observer']['output']],text=True).strip())
for n in ['summary.json','queries.jsonl','ready.json']:
 f=native/n
 if f.exists():(d/('witness-'+n)).write_bytes(f.read_bytes())
body=h.IDENTITY+'''p r3_result
p r3_writer
p r3_clock_fault
p fifo.running
p fifo.error
p fifo.stop_error
p q.count
p r3_card.failed
p r3_card.error
p r3_card.writes
p/x *(unsigned int*)0x42021414
'''
f=d/'current-image.bin';body+=f'dump binary memory {h.win(f).replace(chr(92),"/")} 0x08000000 {hex(0x08000000+(P/"arm/r3_coex.bin").stat().st_size)}\n'
body+='''if r3_result.stage!=99 || r3_result.run!=5 || !r3_result.error || fifo.running || r3_writer.active || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end
'''
tree=ast.parse((P/'run_capture.py').read_text());dumps=None
for n in ast.walk(tree):
 if isinstance(n,ast.Assign) and any(isinstance(t,ast.Name) and t.id=='dumps' for t in n.targets):dumps=ast.literal_eval(n.value);break
assert dumps
for name,start,end in dumps:
 f=d/(name+'.bin');f.write_bytes(b'');body+=f'if {end} > {start}\ndump binary memory {h.win(f).replace(chr(92),"/")} {start} {end}\nend\n'
h.debug('long-error-terminal-readonly-collect',P/'arm/r3_coex.elf',body,timeout=60)
assert (d/'current-image.bin').read_bytes()==(P/'arm/r3_coex.bin').read_bytes()
r=c.analyze(d);out=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),result=r,image_entire_BIN_identical=True,FIFO_stopped=True,writer_inactive=True,SD_CS_high=True,GDB_detached=True,no_reset=True,no_flash=True,no_new_START=True,producer_sha256=(d/'source-sha.bin').read_bytes().hex())
(d/'STATUS.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out,indent=2))
