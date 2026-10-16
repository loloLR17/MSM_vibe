from pathlib import Path
import json,hashlib,subprocess as s,datetime
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4Q-live-b3-rtu');p=Path('/home/lolo/dev/msm/reports/TR2-r4q');initial=json.loads((p/'initial-state.json').read_text())
for f in initial['protected']:assert hashlib.sha256((root/f['path']).read_bytes()).hexdigest()==f['sha256'],f['path']
blocks=s.check_output(['git','worktree','list','--porcelain'],cwd=root,text=True).strip().split('\n\n');count=0
for b in initial['worktrees'].strip().split('\n\n'):
 if b.startswith('worktree '+str(root)+'\n'):continue
 assert b in blocks,b;count+=1
stashes=s.check_output(['git','stash','list','--format=%H'],cwd=root,text=True).splitlines();assert stashes==initial['stashes']
original=Path('/home/lolo/dev/msm/projects/MSM_vibe');assert s.check_output(['git','status','--porcelain'],cwd=original,text=True)==''
assert s.check_output(['git','rev-parse','HEAD'],cwd=original,text=True).strip()=='82169af7e76797731358852bec83bfb0ceb54ccf'
ex='Modbus RTU/00_gouvernance/Echanges_Codex/'
for a,b in [('MISSION_EN_COURS.md','MISSION.md'),('DERNIER_RAPPORT.md','RAPPORT.md')]:
 assert (root/ex/'archives/TR2-20261016-R4P-PHYSICAL-COEXISTENCE'/b).read_bytes()==s.check_output(['git','show','48805fe:'+ex+a],cwd=root)
old=s.check_output(['git','ls-tree','-r','--name-only',initial['initial_head']],cwd=root,text=True).splitlines();old_proofs=0
for f in old:
 if '/preuves/' in f:
  assert (root/f).read_bytes()==s.check_output(['git','show',initial['initial_head']+':'+f],cwd=root);old_proofs+=1
r=dict(result='PASS',protected_files=len(initial['protected']),old_proofs_exact=old_proofs,old_worktrees_exact=count,stashes_exact=stashes,original_main_clean_and_preserved=True,R4P_archives_exact=True,utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
(p/'preservation.json').write_text(json.dumps(r,indent=2)+'\n');print(r)
