from pathlib import Path
import subprocess as s,tarfile,io,json,hashlib,datetime
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4P-physical-coexistence');s.run(['git','fetch','origin','main'],cwd=root,check=True,capture_output=True)
sha=s.check_output(['git','rev-parse','origin/main'],cwd=root,text=True).strip();head=s.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip();assert sha==head
names=s.check_output(['git','diff','--name-only','03309c076da7322fb774d41c032112cd663390fd',sha],cwd=root,text=True).splitlines()
with tarfile.open(fileobj=io.BytesIO(s.check_output(['git','archive',sha],cwd=root))) as archive:
 for name in names:assert archive.extractfile(name).read()==(root/name).read_bytes(),name
 prefix='Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261016-R4P/'
 manifest=json.loads(archive.extractfile(prefix+'SHA256SUMS.json').read())['files']
 for name,record in manifest.items():
  data=archive.extractfile(prefix+name).read();assert len(data)==record['bytes'] and hashlib.sha256(data).hexdigest()==record['sha256'],name
result=dict(sha=sha,utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),fully_read_files=names,verified_proofs=len(manifest),method='fetch GitHub origin/main; full git archive content comparison and full proof hashes')
Path('/home/lolo/dev/msm/reports/TR2-r4p/latest-readback.json').write_text(json.dumps(result,indent=2)+'\n');print('REMOTE_FULLY_READ',sha,len(names),len(manifest))
