from pathlib import Path
import subprocess as s,json,hashlib,datetime
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4D-start-timing');local=Path('/home/lolo/dev/msm/reports/TR2-r4d');initial=json.loads((local/'initial-state.json').read_text())
for f in initial['protected']:assert hashlib.sha256((root/f['path']).read_bytes()).hexdigest()==f['sha256'],f['path']
original=Path('/home/lolo/dev/msm/projects/MSM_vibe')
assert s.check_output(['git','rev-parse','HEAD'],cwd=original,text=True).strip()==initial['original_main_head']
assert s.check_output(['git','status','--porcelain'],cwd=original,text=True)==initial['original_main_status']
stashes=s.check_output(['git','stash','list','--format=%H'],cwd=root,text=True).splitlines();assert stashes==initial['stashes']
new=s.check_output(['git','worktree','list','--porcelain'],cwd=root,text=True)
oldblocks=initial['worktrees'].strip().split('\n\n');newblocks=new.strip().split('\n\n');same=0
for block in oldblocks:
 if block.startswith('worktree '+str(root)+'\n'):continue
 assert block in newblocks,block;same+=1
ex='Modbus RTU/00_gouvernance/Echanges_Codex/'
for a,b in [('MISSION_EN_COURS.md','MISSION.md'),('DERNIER_RAPPORT.md','RAPPORT.md')]:
 expected=s.check_output(['git','show','71ee541d6f534d993d439475c32bec5b03d7856c:'+ex+a],cwd=root)
 assert (root/ex/'archives/TR2-20261016-R4M-LIVE-FRAM-RECONCILIATION'/b).read_bytes()==expected
r=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),protected_files_exact=len(initial['protected']),old_worktrees_unchanged=same,total_worktrees=len(newblocks),stashes_unchanged=stashes,original_main_unchanged=True,R4M_archives_exact=True)
(local/'preservation-summary.json').write_text(json.dumps(r,indent=2)+'\n');print(r)
