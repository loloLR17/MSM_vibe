from pathlib import Path
import hashlib,json,subprocess as s,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-r4u');r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4U-live-runtime');initial=json.loads((p/'initial-state.json').read_text())
def git(*a,cwd=r):return s.check_output(['git',*a],cwd=cwd,text=True).strip()
for f in initial['protected']:assert hashlib.sha256((r/f['path']).read_bytes()).hexdigest()==f['sha256'],f['path']
assert git('stash','list','--format=%H').splitlines()==initial['stashes']
a=initial['worktrees'].strip().split('\n\n');b=git('worktree','list','--porcelain').split('\n\n');n=0
for block in a:
 if block.startswith('worktree '+str(r)+'\n'):continue
 assert block in b,block;n+=1
root=r.parent/'MSM_vibe';assert git('rev-parse','HEAD',cwd=root)=='82169af7e76797731358852bec83bfb0ceb54ccf' and not git('status','--porcelain',cwd=root)
assert git('rev-parse','codex/r4s-retirement-study')=='86d0f4b2b47ee8160bb459d4c19a2a78c7f4d846'
v=dict(result='PASS',protected_files=len(initial['protected']),preexisting_other_worktrees_unchanged=n,stashes=initial['stashes'],original_clean=True,original_head=git('rev-parse','HEAD',cwd=root),local_study_preserved=True,utc=datetime.datetime.now(datetime.timezone.utc).isoformat());(p/'preservation-final.json').write_text(json.dumps(v,indent=2)+'\n');print(v)
