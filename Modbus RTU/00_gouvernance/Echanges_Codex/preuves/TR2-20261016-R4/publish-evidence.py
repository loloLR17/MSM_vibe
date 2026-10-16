from pathlib import Path
import json,hashlib,shutil
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4-cooperative-fram');out=root/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261016-R4';src=Path('/home/lolo/dev/msm/reports/TR2-r4')
for f in src.glob('*.log'):
 if f.stat().st_size<400000:shutil.copy2(f,out/f.name)
for name in ['loc-code.json','module-inventory.json','r4-resources.py','r4-inventory.py','checkpoint-readback.json','preservation-final.json','publish-evidence.py','readback.py']:
 if (src/name).exists():shutil.copy2(src/name,out/name)
for dirname in ['hardware','resources']:
 dest=out/dirname;dest.mkdir(exist_ok=True)
 for f in (src/dirname).iterdir():
  if f.is_file() and f.suffix in ['.log','.gdb','.json','.py','.txt','.su','.c']:
   shutil.copy2(f,dest/f.name)
files={str(f.relative_to(out)):dict(bytes=f.stat().st_size,sha256=hashlib.sha256(f.read_bytes()).hexdigest()) for f in sorted(out.rglob('*')) if f.is_file() and f.name!='SHA256SUMS.json'}
(out/'SHA256SUMS.json').write_text(json.dumps(dict(method='SHA256 of full file bytes; manifest excludes itself; backups/binaries local only',files=files),indent=2)+'\n')
print('EVIDENCE',len(files))
