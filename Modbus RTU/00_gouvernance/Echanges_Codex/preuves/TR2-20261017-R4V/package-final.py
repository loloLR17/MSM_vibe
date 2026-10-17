from pathlib import Path
import hashlib,zlib,json,datetime
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4V-rtu-latency');p=Path('/home/lolo/dev/msm/reports/TR2-r4v');out=r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261017-R4V';bundle=[]
def sha(b):return hashlib.sha256(b).hexdigest()
def put(src,dst=None,compress=False):
 src=Path(src);raw=src.read_bytes();name=(dst or src.name)+('.zlib' if compress else '');target=out/name;target.parent.mkdir(parents=True,exist_ok=True);data=zlib.compress(raw,9) if compress else raw
 if target.exists():assert target.read_bytes()==data,name
 else:target.write_bytes(data)
 bundle.append(dict(path=name,stored_bytes=len(data),stored_sha256=sha(data),raw_bytes=len(raw),raw_sha256=sha(raw),zlib=compress))
for name in ['paced-summary.json','comparison-final.json','final-fram-audit.json','preservation-final.json','check-preservation.py','verify-final-offline.py','candidate-readback.json','build-probes.py','BenchMaster.exe','master-build.log','sanitizer-configure.log','sanitizer-build.log','sanitizer-final-build.log','host-configure.log','quality-build-red.log','quality-build-green.log','model-baseline.bin','model-paced.bin','package-final.py']:
 put(p/name,compress=Path(name).suffix in ['.exe','.bin'])
put(p/'images.json','images-final.json');put(p/'initial-state.json',compress=True)
for src in sorted((p/'hardware').iterdir()):
 if src.suffix in ['.log','.gdb','.json','.py']:put(src,'hardware/'+src.name)
 if src.suffix in ['.bin','.jsonl']:put(src,'hardware/'+src.name,True)
for mode in ['0','250']:
 b=p/('probe-'+mode)
 for suffix in ['.bin','.elf']:put(b/('tr2-r4v-latency-NON-PRODUCTION'+suffix),'images/profile'+mode+suffix,True)
 put(b/'build-profile.txt','images/profile'+mode+'-build-profile.txt')
for suffix in ['bin','elf']:put(p/('unflashed-paced01.'+suffix+'.zlib'),'images/unflashed-paced01.'+suffix+'.zlib')
put(r/'Modbus RTU/05_Firmware/build-stm32-cumulative/artifact-sha256.txt','canonical-artifact-sha256.txt')
(out/'BUNDLE.json').write_text(json.dumps(dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),files=bundle,comment='R4V final evidence; raw bytes retained, optional zlib compression only. Baseline f8bf7ca and candidate8db2f67 really evaluated. Unflashed variant separate. Models metadata-only; all actual effects in MCU and master traces.'),indent=2)+'\n')
files=sorted(f for f in out.rglob('*') if f.is_file() and f.name!='SHA256SUMS');(out/'SHA256SUMS').write_text(''.join(sha(f.read_bytes())+'  '+str(f.relative_to(out))+'\n' for f in files))
print('BUNDLE',len(bundle),'manifest',len(files),'stored bytes',sum(f.stat().st_size for f in files))
