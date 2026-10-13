from pathlib import Path
import subprocess,json,hashlib,datetime
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-M017');base='df5050f41c180d080529229cfd836cb55a230b0e'
def git(*a):return subprocess.check_output(['git',*a],cwd=r)
sha=git('rev-parse','origin/main').decode().strip();assert sha==git('rev-parse','HEAD').decode().strip();assert sha==git('ls-remote','origin','refs/heads/main').decode().split()[0];assert not git('status','--porcelain')
files=git('diff','--name-only','-z',base,sha).decode().split('\0')[:-1];checks=[]
allowed=['Modbus RTU/00_gouvernance/ETAT_COURANT_TR2.md','Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md','experiments/sdmmc_st_reference/src/load_main.c','experiments/sdmmc_st_reference/M017.md','experiments/sdmmc_st_reference/run_m017.py','experiments/sdmmc_st_reference/verify_m017_prefix.py']
for f in files:
 assert f in allowed or f.startswith('Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261014-M017-SDMMC-1BIT/') or f.startswith('Modbus RTU/00_gouvernance/Echanges_Codex/archives/TR2-20261013-M016-SDMMC-1BIT-LOAD-ONLY/')
 b=git('show',sha+':'+f);assert b==(r/f).read_bytes();checks.append({'path':f,'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest()})
pub=r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261014-M017-SDMMC-1BIT'
for name,h in json.loads((pub/'manifest.json').read_text()).items():assert hashlib.sha256((pub/name).read_bytes()).hexdigest()==h
for f in ['AGENTS.md','Modbus RTU/00_gouvernance/Echanges_Codex/MISSION_EN_COURS.md','Modbus RTU/00_gouvernance/Echanges_Codex/PROTOCOLE_ECHANGE_V2.md','Modbus RTU/00_gouvernance/GOUVERNANCE_CHATGPT_CODEX.md','Modbus RTU/00_gouvernance/DETTE_TECHNIQUE_A04.md']:
 assert git('show',sha+':'+f)==git('show',base+':'+f)
assert git('stash','list','--format=%H').decode().splitlines()==['6251b988e8b20f97997a8f98fd26d4d35a45a1b2','f3b829e6188f491920edf74a5f9ed0f559d08b87']
original=Path('/home/lolo/dev/msm/projects/MSM_vibe');assert subprocess.check_output(['git','rev-parse','HEAD'],cwd=original).decode().strip()=='82169af7e76797731358852bec83bfb0ceb54ccf';assert not subprocess.check_output(['git','status','--porcelain'],cwd=original)
report=git('show',sha+':Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md').decode();assert 'mission_id: TR2-20261014-M017-SDMMC-1BIT-REALISTIC-QUALIFICATION' in report
p=Path('/home/lolo/dev/msm/reports/TR2-M017-20261014');(p/f'remote-readback-{sha[:8]}.json').write_text(json.dumps({'mission_id':'TR2-20261014-M017-SDMMC-1BIT-REALISTIC-QUALIFICATION','remote_sha':sha,'utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'method':'fetch GitHub main + ls-remote; full git-show byte comparison for every changed file; manifest and protected files verified','files':checks,'worktree_clean':True,'original_head_preserved':True,'original_worktree_clean':True,'stashes_preserved':True},indent=2)+'\n')
print('FULL_REMOTE_READBACK_OK',sha,len(files),'files; original clean and unchanged, stashes/gov/debt preserved')
