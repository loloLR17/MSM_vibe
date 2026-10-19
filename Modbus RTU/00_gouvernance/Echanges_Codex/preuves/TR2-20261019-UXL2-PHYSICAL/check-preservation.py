from pathlib import Path
import json,hashlib,subprocess,datetime
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-UXL2-physical-live-ui');p=Path('/home/lolo/dev/msm/reports/TR2-uxl2-physical');initial=json.loads((p/'initial-state.json').read_text());authorized=[]
def git(*a,cwd=r):return subprocess.check_output(['git',*a],cwd=cwd,text=True).strip()
n=0
for f in initial['protected']:
 if f['path'] in authorized:continue
 assert hashlib.sha256((r/f['path']).read_bytes()).hexdigest()==f['sha256'],f['path'];n+=1
assert git('stash','list','--format=%H').splitlines()==initial['stashes']
now=git('worktree','list','--porcelain').split('\n\n');old=initial['worktrees'].strip().split('\n\n');trees=0
for block in old:
 if block.startswith('worktree '+str(r)+'\n'):continue
 assert block in now,block;trees+=1
root=r.parent/'MSM_vibe';assert git('rev-parse','HEAD',cwd=root)=='82169af7e76797731358852bec83bfb0ceb54ccf' and not git('status','--porcelain',cwd=root)
assert git('rev-parse','codex/r4s-retirement-study')=='86d0f4b2b47ee8160bb459d4c19a2a78c7f4d846'
v=dict(result='PASS',protected_files=n,other_worktrees_unchanged=trees,stashes=initial['stashes'],original_head=git('rev-parse','HEAD',cwd=root),original_clean=True,authorized_factual_updates=authorized,utc=datetime.datetime.now(datetime.timezone.utc).isoformat());(p/'preservation.json').write_text(json.dumps(v,indent=2)+'\n');print(v)
