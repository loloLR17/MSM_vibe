import json,subprocess,hashlib,csv
from pathlib import Path
P=Path(__file__).parent;d=Path('Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261012-M015-M')
for f in P.iterdir():
 if f.is_file() and (f.suffix in ('.log','.gdb','.py','.csv') or f.name.endswith(('-analysis.json','-invocation.json','-flash-command.json')) or f.name in ('cleanup-artifacts.json','fifo-integrity.json','wiring-confirmed.json')):
  raw=f.read_bytes()
  try:text=raw.decode('utf-8')
  except UnicodeDecodeError:
   (d/(f.name+'.original.hex')).write_text(raw.hex()+'\n');text=raw.decode('cp1252')
  (d/f.name).write_text('\n'.join(x.rstrip() for x in text.splitlines())+'\n')
 if f.is_file() and f.suffix=='.bin':(d/(f.name+'.hex')).write_text(f.read_bytes().hex()+'\n')
for o in ('fifo-O0','fifo-O2','cleanup-O2'):
 for kind in ('trace','preclose'):
  rows=json.loads((P/f'{o}-{kind}-records.json').read_text())
  with (d/f'{o}-{kind}.csv').open('w',newline='') as f:w=csv.DictWriter(f,fieldnames=list(rows[0]),lineterminator='\n');w.writeheader();w.writerows(rows)
 src=P/o/'iis3dwb_reference.bin';(d/(o+'-image.bin.hex')).write_text(src.read_bytes().hex()+'\n')
 comp=subprocess.check_output(['ninja','-C',str(P/o),'-t','commands'],text=True)
 (d/f'{o}-spi-compile.txt').write_text('\n'.join(x for x in comp.splitlines() if ' -c ' in x and 'stm32u5xx_hal_spi_observed.c' in x)+'\n')
for o in (0,2):
 lines=(P/f'fifo-O{o}-receive-disassembly.txt').read_text().splitlines()
 selected=[x for x in lines if (o==0 and ('8001d' in x or '8001ee' in x or '8001ef' in x)) or (o==2 and ('80018e' in x or '80018f' in x or '800190' in x or '800191' in x))]
 (d/f'fifo-O{o}-rx-loop.txt').write_text('\n'.join(x.rstrip() for x in selected)+'\n')
initial=json.loads((P/'initial.json').read_text())
for section in initial['worktrees'].split('\n\n'):
 lines=section.splitlines()
 if not lines:continue
 path=lines[0].removeprefix('worktree ')
 if path==str(Path.cwd()):continue
 assert subprocess.check_output(['git','-C',path,'status','--porcelain'],text=True)==''
 assert subprocess.check_output(['git','-C',path,'rev-parse','HEAD'],text=True).strip()==lines[1].removeprefix('HEAD ')
assert subprocess.check_output(['git','stash','list','--format=%H'],text=True).splitlines()==initial['stashes']
vendor=Path('experiments/iis3dwb_st_reference/vendor/SOURCES.json');sources=json.loads(vendor.read_text());base=vendor.parent.parent
for x in sources['files']:assert hashlib.sha256((base/x['path']).read_bytes()).hexdigest()==x['sha256']
preserved={'historical_worktrees_heads_and_clean_status':'UNCHANGED','stashes':'UNCHANGED','vendor_hashes':'EXACT','primary_access':'NONE','flash2MiB_backup_restore':'NONE','final_sensor_readbacks':'CTRL1_XL=0C (XL off), FIFO_CTRL4=80 (bypass), CTRL10_C=00 (timestamp off)','image_readback':[hashlib.sha256((P/f'{n}-flash.bin').read_bytes()).hexdigest() for n in ('fifo-O0','fifo-O2','cleanup-O2')]}
(P/'preservation.json').write_text(json.dumps(preserved,indent=2)+'\n');(d/'preservation.json').write_bytes((P/'preservation.json').read_bytes())
manifest={f.name:{'bytes':f.stat().st_size,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()} for f in d.iterdir() if f.is_file() and f.name!='manifest.json'};(d/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
print('Public evidence:',len(manifest),'files; preserved worktrees/stashes/vendor; exact three image readbacks')
