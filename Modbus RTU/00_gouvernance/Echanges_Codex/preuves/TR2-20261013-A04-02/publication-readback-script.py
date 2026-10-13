from pathlib import Path
import subprocess,json,hashlib,datetime
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-A04-02');p=Path(Path('/tmp/tr2-a0402-proof').read_text());sha=subprocess.check_output(['git','rev-parse','origin/main'],cwd=r,text=True).strip();assert sha==subprocess.check_output(['git','rev-parse','HEAD'],cwd=r,text=True).strip();actual=subprocess.check_output(['git','ls-remote','origin','refs/heads/main'],cwd=r,text=True).split()[0];assert actual==sha
files=subprocess.check_output(['git','diff','--name-only','2cbb9bbc',sha],cwd=r,text=True).splitlines();record={}
for f in files:
 data=subprocess.check_output(['git','show',sha+':'+f],cwd=r);assert data==(r/f).read_bytes(),f;record[f]=hashlib.sha256(data).hexdigest()
(p/('readback-'+sha[:8]+'.json')).write_text(json.dumps(dict(sha=sha,ls_remote_sha=actual,created_at_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),full_byte_identical=True,files=record),indent=2)+'\n');print(sha,len(record),'files read fully and matched')
