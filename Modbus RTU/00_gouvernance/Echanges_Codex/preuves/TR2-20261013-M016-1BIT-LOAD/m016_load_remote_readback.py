from pathlib import Path
import subprocess, json, hashlib, datetime
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-M016-1bit-load-only'); base='5efffbd84950cbe96d114b0619fd8846b07d63f0'
def git(*a): return subprocess.check_output(['git',*a],cwd=r)
sha=git('rev-parse','origin/main').decode().strip()
assert sha==git('rev-parse','HEAD').decode().strip()
assert git('ls-remote','origin','refs/heads/main').decode().split()[0]==sha
assert not git('status','--porcelain')
files=git('diff','--name-only','-z',base,sha).decode().split('\0')[:-1]
checks=[]
for f in files:
 b=git('show',sha+':'+f); assert b==(r/f).read_bytes()
 checks.append({'path':f,'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest()})
pub=r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261013-M016-1BIT-LOAD'
for name,h in json.loads((pub/'manifest.json').read_text()).items(): assert hashlib.sha256((pub/name).read_bytes()).hexdigest()==h
report=git('show',sha+':Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md').decode()
assert 'mission_id: TR2-20261013-M016-SDMMC-1BIT-LOAD-ONLY' in report
for f in ['AGENTS.md','Modbus RTU/00_gouvernance/Echanges_Codex/MISSION_EN_COURS.md','Modbus RTU/00_gouvernance/Echanges_Codex/PROTOCOLE_ECHANGE_V2.md','Modbus RTU/00_gouvernance/DETTE_TECHNIQUE_A04.md']:
 assert git('show',sha+':'+f)==git('show',base+':'+f)
# No production source change in mission: all changed paths explicitly allowed.
for f in files:
 assert f.startswith('Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261013-M016-1BIT-LOAD/') or f.startswith('Modbus RTU/00_gouvernance/Echanges_Codex/archives/TR2-20261013-M016-GPIO-DAT123-UNIT/') or f in ['Modbus RTU/00_gouvernance/ETAT_COURANT_TR2.md','Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md','experiments/sdmmc_st_reference/CMakeLists.txt','experiments/sdmmc_st_reference/LOAD_ONLY.md','experiments/sdmmc_st_reference/src/load_main.c']
stashes=git('stash','list','--format=%H').decode().splitlines()
assert stashes==['6251b988e8b20f97997a8f98fd26d4d35a45a1b2','f3b829e6188f491920edf74a5f9ed0f559d08b87']
original=Path('/home/lolo/dev/msm/projects/MSM_vibe')
assert subprocess.check_output(['git','rev-parse','HEAD'],cwd=original).decode().strip()=='82169af7e76797731358852bec83bfb0ceb54ccf'
assert not subprocess.check_output(['git','status','--porcelain'],cwd=original)
result={'mission_id':'TR2-20261013-M016-SDMMC-1BIT-LOAD-ONLY','remote_sha':sha,'utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'method':'fetch GitHub main + ls-remote SHA; read full git-show content of each changed file and byte compare to worktree; manifest and protected files checked','files':checks,'git_worktree_clean':True,'original_head_unchanged':True,'original_worktree_clean':True,'two_stashes_preserved':True}
p=Path('/home/lolo/dev/msm/reports/TR2-M016-1BIT-LOAD-20261013T215507Z')
(p/('remote-readback-'+sha[:8]+'.json')).write_text(json.dumps(result,indent=2)+'\n')
print('FULL_REMOTE_READBACK_OK',sha,len(files),'files; clean worktrees, protected files and two stashes preserved')
