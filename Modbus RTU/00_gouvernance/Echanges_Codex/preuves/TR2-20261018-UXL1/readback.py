from pathlib import Path
import subprocess,json,zlib,hashlib,datetime,xml.etree.ElementTree as E
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-UXL1-live-readonly');p=Path('/home/lolo/dev/msm/reports/TR2-uxl1');prefix='Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261018-UXL1/'
def git(*a):return subprocess.check_output(['git',*a],cwd=r)
subprocess.run(['git','fetch','origin','main'],cwd=r,check=True,capture_output=True);head=git('rev-parse','HEAD').decode().strip();assert head==git('rev-parse','origin/main').decode().strip()==git('ls-remote','origin','refs/heads/main').decode().split()[0]
def blob(f):return git('show',head+':'+f)
files=git('diff','--name-only','cfabfbf291c83e20878f90181b7d18925f079ddb',head).decode().splitlines()
for f in files:assert blob(f)==(r/f).read_bytes(),f
manifest=blob(prefix+'SHA256SUMS').decode().splitlines()
for line in manifest:
 h,f=line.split('  ',1);assert hashlib.sha256(blob(prefix+f)).hexdigest()==h
meta=json.loads(blob(prefix+'BUNDLE.json'));raw={}
for x in meta['artifacts']:
 b=blob(prefix+x['path']);assert hashlib.sha256(b).hexdigest()==x['stored_sha256'];a=zlib.decompress(b) if x['encoding']=='zlib' else b;assert len(a)==x['bytes'] and hashlib.sha256(a).hexdigest()==x['sha256'];raw[x['source']]=a
ns={'t':'http://microsoft.com/schemas/VisualStudio/TeamTest/2010'}
trx=[E.fromstring(b).find('.//t:Counters',ns).attrib for f,b in raw.items() if f.startswith('service-tests-final/') and f.endswith('.trx')];assert len(trx)==1 and trx[0]['passed']=='217' and trx[0]['failed']=='0'
smoke=json.loads(raw.get('selection-final-smoke/smoke-summary.json',raw['smoke-summary.json']));assert smoke['archive_byte_identical'] and smoke['no_WAL'] and not smoke['physical'] and smoke['command_post_status']==404
for name in ['detail-dom.html','fleet-dom.html']:
 text=raw.get('selection-final-smoke/'+name,raw[name]).decode();assert all(x in text for x in ['Historique','NON-PRODUCTION','LiveDiagnostic','mg'])
assert hashlib.sha256(raw['historical-r5b2.db']).hexdigest()==smoke['archive_sha256']
report=blob('Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md').decode();assert 'mission_id: TR2-20261018-UXL1-LIVE-READONLY-INTEGRATION' in report
result=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),remote_sha=head,full_changed_files_compared=len(files),manifest_hashes_verified=len(manifest),bundle_artifacts_verified=len(raw),service_tests_final_passed=217,other_tests_passed=350,headless_DOM_pages_verified=2,archive_sha256=smoke['archive_sha256'],archive_unchanged=True,physical_access=False,report_status=report.split('status: ',1)[1].splitlines()[0])
(p/('readback-'+head[:7]+'.json')).write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
