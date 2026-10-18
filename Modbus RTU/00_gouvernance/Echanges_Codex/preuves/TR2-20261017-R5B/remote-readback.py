from pathlib import Path
import subprocess,hashlib,zlib,json,datetime,xml.etree.ElementTree as E
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R5B-live-to-web');p=Path('/home/lolo/dev/msm/reports/TR2-r5b');base='868a80dcba90f794aa694cc470fca6a421292a2e';proof='Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261017-R5B/'
def git(*a):return subprocess.check_output(['git',*a],cwd=r)
def blob(f):return git('show',remote+':'+f)
subprocess.run(['git','fetch','origin','main'],cwd=r,check=True,capture_output=True);remote=git('rev-parse','origin/main').decode().strip();head=git('rev-parse','HEAD').decode().strip();live=git('ls-remote','origin','refs/heads/main').decode().split()[0];assert remote==head==live
files=git('diff','--name-only',base,remote).decode().splitlines()
for f in files:assert blob(f)==(r/f).read_bytes(),f
lines=blob(proof+'SHA256SUMS').decode().splitlines()
for x in lines:
 h,f=x.split('  ',1);assert hashlib.sha256(blob(proof+f)).hexdigest()==h,f
meta=json.loads(blob(proof+'BUNDLE.json'));raw={}
for x in meta['artifacts']:
 b=blob(proof+x['path']);assert hashlib.sha256(b).hexdigest()==x['stored_sha256'];a=zlib.decompress(b) if x['encoding']=='zlib' else b;assert len(a)==x['bytes'] and hashlib.sha256(a).hexdigest()==x['sha256'];raw[x['source']]=a
assert hashlib.sha256(raw['hardware/flash-before.bin']).hexdigest()=='c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26'
assert hashlib.sha256(raw['hardware/fram-before.bin']).hexdigest()=='4e7fabde5da1100cf9ae8faf262ea079727856826bb2829c1e99b68ad63b84bd'
ns={'t':'http://microsoft.com/schemas/VisualStudio/TeamTest/2010'};tests=[E.fromstring(a).find('.//t:Counters',ns).attrib for f,a in raw.items() if f.endswith('.trx')];assert sum(int(x['passed']) for x in tests)==561 and all(x['failed']=='0' and x['notExecuted']=='0' for x in tests)
events=json.loads(raw['windows-code-integrity.json']);assert any(x['Id']==3077 and 'TR2.Protocol.dll' in x['Message'] for x in events)
report=blob('Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md').decode();assert 'mission_id: TR2-20261017-R5B-LIVE-PHYSICAL-TO-WEB-DEMO' in report and 'status: BLOCKED' in report
result=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),remote_sha=remote,ls_remote_sha=live,changed_files_full_contents_compared=len(files),manifest_entries_verified=len(lines),bundle_artifacts_verified=len(meta['artifacts']),dotnet_tests_passed=561,hardware_trial=False,code_integrity_3077_verified=True,flash_before_sha256=hashlib.sha256(raw['hardware/flash-before.bin']).hexdigest(),fram_before_sha256=hashlib.sha256(raw['hardware/fram-before.bin']).hexdigest(),report_status='BLOCKED',note='Full blob reads from freshly fetched GitHub main; delivery verified, physical mission still blocked')
(p/('readback-'+remote[:7]+'.json')).write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
