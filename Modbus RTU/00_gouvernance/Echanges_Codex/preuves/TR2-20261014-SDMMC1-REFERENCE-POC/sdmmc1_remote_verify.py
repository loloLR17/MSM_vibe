from pathlib import Path
import subprocess,json,hashlib,datetime
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-SDMMC1-PoC')
def git(*args,cwd=root): return subprocess.check_output(['git',*args],cwd=cwd)
base='3073d95b1fcf9d6e3e3ed5c35307a17364bdf3d5'
sha=git('rev-parse','origin/main').decode().strip()
assert sha==git('rev-parse','HEAD').decode().strip()
assert git('ls-remote','origin','refs/heads/main').decode().split()[0]==sha
paths=git('diff','--name-only','-z',base,sha).decode().split('\0')[:-1]
read=[]
for name in paths:
 data=git('show',sha+':'+name)
 assert data==(root/name).read_bytes(),name
 read.append({'path':name,'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest()})
p=root/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261014-SDMMC1-REFERENCE-POC'
manifest=json.loads((p/'manifest.json').read_text())
for name,h in manifest.items(): assert hashlib.sha256((p/name).read_bytes()).hexdigest()==h,name
original=Path('/home/lolo/dev/msm/projects/MSM_vibe')
assert not git('status','--porcelain=v1',cwd=original)
assert git('rev-parse','HEAD',cwd=original).decode().strip()=='82169af7e76797731358852bec83bfb0ceb54ccf'
stashes=git('stash','list','--format=%H',cwd=original).decode().splitlines()
assert stashes==['6251b988e8b20f97997a8f98fd26d4d35a45a1b2','f3b829e6188f491920edf74a5f9ed0f559d08b87'],stashes
src=Path('/home/lolo/dev/msm/cache/VILLO88-NUCLEOU575-SDMMC1-PoC')
assert not git('status','--porcelain=v1',cwd=src)
assert git('rev-parse','HEAD',cwd=src).decode().strip()=='1e3c9183bad023ebf7d234134823d9270a0e8b64'
result={'verified_at_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'remote_sha':sha,'base_sha':base,'all_changed_files_read_and_byte_equal':True,'changed_file_count':len(paths),'manifest_entries_verified':len(manifest),'original_worktree_clean_and_preserved':True,'stashes':stashes,'upstream_source_clean_and_pinned':True,'files':read}
out=Path('/home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014')/('remote-readback-'+sha[:8]+'.json')
out.write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps({k:v for k,v in result.items() if k!='files'},indent=2))
print(out)
