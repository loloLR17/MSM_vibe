"""Independent host-only counterexample using main's UNCHANGED R4M/R2.
Run from a repo root; optional --reuse-build points to an existing main core.
No STM32 tool, flash, serial port or media access is invoked.
"""
from pathlib import Path
import tempfile,subprocess as s,json,zlib,hashlib,argparse,datetime
cli=argparse.ArgumentParser();cli.add_argument('--reuse-build',type=Path);a=cli.parse_args()
root=Path.cwd();fw=root/'Modbus RTU/05_Firmware';local=Path(__file__).resolve().parent
proof=root/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261016-R4S'
source=local/'counterexample_model.c';snapshot=local/'hardware/fram-before.bin'
if snapshot.exists():raw=snapshot.read_bytes()
else:raw=zlib.decompress((proof/'hardware/fram-before.bin.zlib').read_bytes())
assert len(raw)==262144 and hashlib.sha256(raw).hexdigest()=='14fb172f2917d0a8e440438051831c84f3f3e746161702a7de9e9a1a251b2aa9'
with tempfile.TemporaryDirectory(prefix='r4s-counterexample-') as tmp:
 tmp=Path(tmp);b=a.reuse_build or tmp/'build';commands=[]
 if not a.reuse_build:
  for c in [['cmake','-S',str(fw),'-B',str(b)],['cmake','--build',str(b),'--target','tr2_core','-j2']]:
   commands.append(c);s.run(c,stdout=s.PIPE,stderr=s.STDOUT,check=True)
 assert (b/'libtr2_core.a').exists();snap=tmp/'snapshot.bin';snap.write_bytes(raw)
 c=['cc','-std=c11','-Wall','-Wextra','-Wpedantic','-Werror','-I'+str(fw/'include'),'-I'+str(fw/'platform/stm32'),'-I'+str(fw/'tests/platform'),str(source),str(fw/'platform/stm32/r4m_maintenance.c'),str(b/'libtr2_core.a'),'-o',str(tmp/'model')];commands.append(c);s.run(c,capture_output=True,check=True)
 c=[str(tmp/'model'),str(snap)];commands.append(c);v=s.run(c,capture_output=True,text=True);assert v.returncode==7,v.stdout+v.stderr;r=json.loads(v.stdout);assert [r['journal_before'],r['journal_bridge'],r['journal_after_renew']]==[26,0,4]
 r.update(counter_preservation_criterion='FAILED',diagnostic='counterexample reproduced with unchanged main R4M/R2',model_exit_code=v.returncode,commands=commands,utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
 (local/'counter-contract.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps(r,indent=2))
