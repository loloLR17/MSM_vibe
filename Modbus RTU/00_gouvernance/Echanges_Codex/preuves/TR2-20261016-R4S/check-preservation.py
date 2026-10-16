from pathlib import Path
import json,hashlib,subprocess as s,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-r4s');r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4S-publication');initial=json.loads((p/'initial-state.json').read_text())
def git(*a,cwd=r):return s.check_output(['git',*a],cwd=cwd,text=True).strip()
for f in initial['protected']:assert hashlib.sha256((r/f['path']).read_bytes()).hexdigest()==f['sha256'],f['path']
stashes=git('stash','list','--format=%H').splitlines();assert stashes==initial['stashes']
old=initial['worktrees'].strip().split('\n\n');current=git('worktree','list','--porcelain').split('\n\n');n=0
for block in old:
 if block.startswith('worktree /home/lolo/dev/msm/projects/MSM_vibe-R4S-service-budget-safe-recovery\n'):continue
 assert block in current,block;n+=1
origin=Path('/home/lolo/dev/msm/projects/MSM_vibe');assert git('rev-parse','HEAD',cwd=origin)=='82169af7e76797731358852bec83bfb0ceb54ccf';assert git('status','--porcelain',cwd=origin)==''
a='Modbus RTU/00_gouvernance/Echanges_Codex/archives/TR2-20261016-R4R-PHYSICAL-TIME-B3/'
for name,source in [('MISSION.md','MISSION_EN_COURS.md'),('RAPPORT.md','DERNIER_RAPPORT.md')]:
 data=s.check_output(['git','show','7ebd712:Modbus RTU/00_gouvernance/Echanges_Codex/'+source],cwd=r);assert data==(r/a/name).read_bytes()
assert not (r/'Modbus RTU/05_Firmware/platform/stm32/r4s_maintenance.c').exists()
result=dict(result='PASS',protected_files=len(initial['protected']),preexisting_other_worktrees_unchanged=n,stashes=stashes,original_head=git('rev-parse','HEAD',cwd=origin),original_clean=True,R4R_archives_exact=True,unapproved_study_on_main=False,local_study_commit=git('rev-parse','codex/r4s-retirement-study'),utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
(p/'preservation-final.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
