from pathlib import Path
import json,hashlib,subprocess,datetime
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R5B-live-to-web');p=Path('/home/lolo/dev/msm/reports/TR2-r5b');initial=json.loads((p/'initial-state.json').read_text())
def git(*a,cwd=r):return subprocess.check_output(['git',*a],cwd=cwd,text=True).strip()
for f in initial['protected']:assert hashlib.sha256((r/f['path']).read_bytes()).hexdigest()==f['sha256'],f['path']
assert git('stash','list','--format=%H').splitlines()==initial['stashes']
now=git('worktree','list','--porcelain').split('\n\n');old=initial['worktrees'].split('\n\n');n=0
for block in old:
 if block.startswith('worktree '+str(r)+'\n'):continue
 assert block in now,block;n+=1
root=r.parent/'MSM_vibe';assert git('rev-parse','HEAD',cwd=root)=='82169af7e76797731358852bec83bfb0ceb54ccf' and not git('status','--porcelain',cwd=root)
assert git('rev-parse','codex/r4s-retirement-study')=='86d0f4b2b47ee8160bb459d4c19a2a78c7f4d846'
v=dict(result='PASS',protected_files=len(initial['protected']),other_worktrees_unchanged=n,stashes=initial['stashes'],original_head=git('rev-parse','HEAD',cwd=root),original_clean=True,utc=datetime.datetime.now(datetime.timezone.utc).isoformat());(p/'preservation.json').write_text(json.dumps(v,indent=2)+'\n');print(v)
