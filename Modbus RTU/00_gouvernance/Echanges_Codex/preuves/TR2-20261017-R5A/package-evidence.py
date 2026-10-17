from pathlib import Path
import hashlib,json,zlib,datetime,re
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R5A-pc-live');p=Path('/home/lolo/dev/msm/reports/TR2-r5a');out=r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261017-R5A';assert not out.exists();out.mkdir();bundle=[]
def put(src,name=None,compress=False):
 src=Path(src);raw=src.read_bytes();name=name or src.name
 if compress:name+='.zlib'
 target=out/name;target.parent.mkdir(parents=True,exist_ok=True);data=zlib.compress(raw,9) if compress else raw;target.write_bytes(data)
 if src.suffix not in ['.sqlite','.db']:
  assert not re.search(rb'-----BEGIN (?:RSA |OPENSSH |EC )?PRIVATE KEY|gh[pousr]_[A-Za-z0-9]{30,}|github_pat_[A-Za-z0-9_]{30,}|sk-[A-Za-z0-9]{32,}',raw),name
 bundle.append(dict(path=name,raw_bytes=len(raw),raw_sha256=hashlib.sha256(raw).hexdigest(),stored_bytes=len(data),stored_sha256=hashlib.sha256(data).hexdigest(),zlib=compress))
for name in ['restore.log','build01.log','build-final.log','live01.log','live02.log','live03.log','test-final.log','service-final.log','ui-test.log','wire-mismatch.json','source-provenance.json','preservation.json','dotnet-info.txt','node-version.txt','smoke-server.log','smoke02-server.log','smoke02-driver.log','smoke02-summary.json','smoke02-api-live.json','smoke02-api-v1-fleet.txt','smoke02-live.html.txt','smoke02-live.js.txt','smoke02-.txt','check-preservation.py','smoke-web.py','smoke-web02.py','package-evidence.py']:
 put(p/name)
for src in sorted(p.rglob('*.trx')):put(src,'tests/'+str(src.relative_to(p)),True)
put(p/'initial-state.json',compress=True);put(p/'smoke02-export.sqlite',compress=True)
(out/'BUNDLE.json').write_text(json.dumps(dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),files=bundle,comment='Software-only R5A evidence; genuine historical ADUs referenced unchanged from R4U/V, not transmitted again. Offline HTTP with explicit simulated B4/B6 headers; rejected B3 retained as invalid. Tests do not qualify STM32.'),indent=2)+'\n')
files=sorted(f for f in out.rglob('*') if f.is_file());(out/'SHA256SUMS').write_text(''.join(hashlib.sha256(f.read_bytes()).hexdigest()+'  '+str(f.relative_to(out))+'\n' for f in files));print('Evidence',len(bundle),'artifacts;',sum(x['stored_bytes'] for x in bundle),'bytes')
