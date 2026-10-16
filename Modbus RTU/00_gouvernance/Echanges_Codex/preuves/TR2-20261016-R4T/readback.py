from pathlib import Path
import subprocess as s,json,hashlib,tarfile,io,datetime
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4T-monotonic-recovery');s.run(['git','fetch','origin','main'],cwd=root,check=True,capture_output=True)
sha=s.check_output(['git','rev-parse','origin/main'],cwd=root,text=True).strip();assert sha==s.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip()
names=s.check_output(['git','diff','--name-only','d17c545',sha],cwd=root,text=True).splitlines()
with tarfile.open(fileobj=io.BytesIO(s.check_output(['git','archive',sha],cwd=root))) as archive:
 for n in names:assert archive.extractfile(n).read()==(root/n).read_bytes(),n
 prefix='Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261016-R4T/'
 files=json.loads(archive.extractfile(prefix+'SHA256SUMS.json').read())['files']
 for n,record in files.items():
  data=archive.extractfile(prefix+n).read();assert len(data)==record['bytes'] and hashlib.sha256(data).hexdigest()==record['sha256'],n
r=dict(sha=sha,files_fully_read=names,proofs_verified=len(files),utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),method='fetch GitHub origin/main, compare complete git archive files and SHA256 proof manifest')
Path('/home/lolo/dev/msm/reports/TR2-r4t/latest-readback.json').write_text(json.dumps(r,indent=2)+'\n');print('REMOTE_FULL_READ',sha,len(names),len(files))
