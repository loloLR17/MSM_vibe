from pathlib import Path
import hashlib,json,zlib,datetime,re
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R5AFIX-b3-wire');p=Path('/home/lolo/dev/msm/reports/TR2-r5afix');out=r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261017-R5AFIX';bundle=[]
def put(src,dst=None,compress=False):
 src=Path(src);raw=src.read_bytes();name=(dst or src.name)+('.zlib' if compress else '');f=out/name;f.parent.mkdir(parents=True,exist_ok=True);data=zlib.compress(raw,9) if compress else raw
 if f.exists():assert f.read_bytes()==data,name
 else:f.write_bytes(data)
 if src.suffix not in ['.bin','.elf','.exe']:assert not re.search(rb'-----BEGIN (?:RSA |OPENSSH |EC )?PRIVATE KEY|gh[pousr]_[A-Za-z0-9]{30,}|github_pat_[A-Za-z0-9_]{30,}|sk-[A-Za-z0-9]{32,}',raw),name
 bundle.append(dict(path=name,raw_bytes=len(raw),raw_sha256=hashlib.sha256(raw).hexdigest(),stored_bytes=len(data),stored_sha256=hashlib.sha256(data).hexdigest(),zlib=compress))
for name in ['physical-summary.json','final-fram-audit.json','source-provenance.json','preservation.json','checkpoint-readback.json','analyze-physical.py','check-preservation.py','package-final.py','model-final.c','model-final.log','image.json','BenchMaster.cs','build-probe.py','sanitizer-build.log','sanitizer-configure.log','physical-replay-tests.log','dotnet-tests.log','full-validation.log','full-validation02.log','oracle-build-red.log','oracle-build-red02.log','oracle-build-green.log','oracle-configure.log','oracle-red.log','oracle-red02.log','oracle-green.log','sanitizer-test.log','probe-build.log','t1000-inventory.json','master-selftest.jsonl']:
 put(p/name)
for f in sorted((p/'hardware').iterdir()):
 if f.suffix in ['.py','.json','.gdb','.log']:put(f,'hardware/'+f.name)
 if f.suffix in ['.bin','.jsonl']:put(f,'hardware/'+f.name,True)
for suffix in ['bin','elf']:put(p/('probe/tr2-r5afix-b3-wire-NON-PRODUCTION.'+suffix),'image/probe.'+suffix,True)
put(p/'probe/build-profile.txt','image/build-profile.txt');put(p/'model-final.bin',compress=True);put(p/'initial-state.json',compress=True);put(p/'BenchMaster.exe',compress=True)
for n in ['service.trx','physical-replay.trx']:put(p/n,compress=True)
put(r/'Modbus RTU/05_Firmware/build-stm32-cumulative/artifact-sha256.txt','canonical-artifact-sha256.txt')
(out/'BUNDLE.json').write_text(json.dumps(dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),files=bundle,comment='Raw bytes retained; zlib only for larger artifacts. One physical START5/10 run10 STOP5/11. Firmware29a6f4e, flashreference23c6bd3; PCtest770aef2. Old corrupt captures untouched. Host fixtures/model are not physical effects.'),indent=2)+'\n')
files=sorted(f for f in out.rglob('*') if f.is_file() and f.name!='SHA256SUMS');(out/'SHA256SUMS').write_text(''.join(hashlib.sha256(f.read_bytes()).hexdigest()+'  '+str(f.relative_to(out))+'\n' for f in files));print('Bundle',len(bundle),'artifacts;manifest',len(files),'bytes',sum(f.stat().st_size for f in files))
