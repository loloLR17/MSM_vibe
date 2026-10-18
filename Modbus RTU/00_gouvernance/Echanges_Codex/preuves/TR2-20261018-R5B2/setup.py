from pathlib import Path
import json,hashlib,subprocess,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-r5b2');r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R5B2-physical-resume');old=Path('/home/lolo/dev/msm/reports/TR2-r5b');w=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-r5b');ev=w.parent/'TR2-r5b2-evidence';ev.mkdir(exist_ok=True)
m=json.loads((old/'windows-build-manifest.json').read_text());n=0
for x in m['files']:
 f=w/x['path'];assert f.exists() and hashlib.sha256(f.read_bytes()).hexdigest()==x['sha256'],f;n+=1
(p/'deployment-verification.json').write_text(json.dumps(dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),local_path=str(w),files_verified=n,code_sha=m['code_sha'],full_copy_exact=True,policy_changes=False),indent=2)+'\n')
initial=json.loads((old/'initial-state.json').read_text());files=[x['path'] for x in initial['protected']]+subprocess.check_output(['git','ls-files','Modbus RTU/00_gouvernance/Echanges_Codex/archives/TR2-20261017-R5B-LIVE-PHYSICAL-TO-WEB-DEMO','Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261017-R5B'],cwd=r,text=True).splitlines()
def git(*a):return subprocess.check_output(['git',*a],cwd=r,text=True).strip()
a=dict(initial_head=git('rev-parse','HEAD'),stashes=git('stash','list','--format=%H').splitlines(),worktrees=git('worktree','list','--porcelain'),protected=[dict(path=f,sha256=hashlib.sha256((r/f).read_bytes()).hexdigest()) for f in sorted(set(files))]);(p/'initial-state.json').write_text(json.dumps(a,indent=2)+'\n')
for n in ['hardware.py','preflight.py','restored-sensor.py','restore.py']:
 t=(old/'hardware'/n).read_text().replace('TR2-r5b/','TR2-r5b2/').replace('MSM_vibe-R5B-live-to-web','MSM_vibe-R5B2-physical-resume').replace('TR2-r5b\'','TR2-r5b2\'').replace('R4V','R5AFIX');(p/'hardware'/n).write_text(t)
# Reuse immutable validated probe; no rebuild on unchanged inputs.
b=p/'probe';b.mkdir(exist_ok=True)
for n in ['tr2-r5b-live-to-web-NON-PRODUCTION.bin','tr2-r5b-live-to-web-NON-PRODUCTION.elf','build-profile.txt']:
 (b/n).write_bytes((old/'probe'/n).read_bytes())
(p/'image.json').write_bytes((old/'image.json').read_bytes())
(p/'check-preservation.py').write_text((old/'check-preservation.py').read_text().replace('TR2-r5b','TR2-r5b2').replace('MSM_vibe-R5B-live-to-web','MSM_vibe-R5B2-physical-resume'))
print('PASS local published files',n, 'count',len(m['files']))
