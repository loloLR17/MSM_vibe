from pathlib import Path
import shutil,zlib,hashlib,json,datetime,subprocess
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4U-live-runtime');p=Path('/home/lolo/dev/msm/reports/TR2-r4u');out=r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261017-R4U';bundle=[]
def sha(b):return hashlib.sha256(b).hexdigest()
def put(src,dst=None,compress=False):
 src=Path(src);raw=src.read_bytes();name=dst or src.name;name+=('.zlib' if compress else '');target=out/name;target.parent.mkdir(parents=True,exist_ok=True);data=zlib.compress(raw,9) if compress else raw
 if target.exists():assert target.read_bytes()==data,name
 else:target.write_bytes(data)
 bundle.append(dict(path=name,stored_bytes=len(data),stored_sha256=sha(data),raw_bytes=len(raw),raw_sha256=sha(raw),zlib=compress))
for name in ['physical-final-summary.json','physical-analysis.log','final-fram-audit.json','initial-fram-audit.json','model-final-audit.json','model-final.log','final_metadata_model.c','analyze-final.py','check-preservation.py','preservation-final.json','canonical-duplex-final.log','checkpoint-readback.json']:
 put(p/name)
for src in sorted((p/'hardware').iterdir()):
 if src.suffix in ['.log','.gdb','.json','.py']:
  if src.name=='hardware.py':put(src,'hardware/hardware-stream.py')
  else:put(src,'hardware/'+src.name)
for name in ['flash-before.bin','flash-restored.bin','fram-before.bin','live05-fram.bin','fram-after-restoration.bin','live05-first-xyz.bin']:
 put(p/'hardware'/name,'hardware/'+name,True)
put(p/'hardware/live05-master.jsonl','hardware/live05-master.jsonl',True)
put(p/'model-final.bin',compress=True);put(p/'initial-state.json',compress=True)
for i in range(1,5):
 b=p/('probe-build-%02d'%i)
 put(b/'tr2-r4u-live-runtime-NON-PRODUCTION.bin','images/image%02d.bin'%i,True)
 put(b/'build-profile.txt','images/image%02d-build-profile.txt'%i)
put(p/'probe-build-04/tr2-r4u-live-runtime-NON-PRODUCTION.elf','images/image04.elf',True)
put(p/'BenchMaster02.exe',compress=True)
put(r/'Modbus RTU/05_Firmware/build-stm32-cumulative/artifact-sha256.txt','canonical-artifact-sha256.txt')
(out/'BUNDLE.json').write_text(json.dumps(dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),files=bundle,comment='Raw bytes preserved; zlib only for larger artifacts. Existing evidence retained. Source ELF04/BIN04 evaluated at806d967; models metadata only.'),indent=2)+'\n')
print('Packaged',len(bundle),'files; bytes',sum(x['stored_bytes'] for x in bundle))
