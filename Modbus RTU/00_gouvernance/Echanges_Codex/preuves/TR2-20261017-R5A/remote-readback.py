from pathlib import Path
import subprocess,hashlib,json,zlib,xml.etree.ElementTree as E,datetime,sys,sqlite3
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R5A-pc-live');p=Path('/home/lolo/dev/msm/reports/TR2-r5a');initial=json.loads((p/'initial-state.json').read_text())['head']
def git(*a):return subprocess.check_output(['git',*a],cwd=r)
sha=git('rev-parse','origin/main').decode().strip();assert sha==git('rev-parse','HEAD').decode().strip()==git('ls-remote','origin','refs/heads/main').decode().split()[0]
names=git('diff','--name-only','-z',initial,sha).decode().strip('\0').split('\0');blobs={}
for n in names:
 data=git('show',sha+':'+n);assert data==(r/n).read_bytes(),n;blobs[n]=data
def blob(n):return blobs[n] if n in blobs else git('show',sha+':'+n)
prefix='Modbus RTU/00_gouvernance/Echanges_Codex/';proof=prefix+'preuves/TR2-20261017-R5A/';report=blob(prefix+'DERNIER_RAPPORT.md').decode();assert 'mission_id: TR2-20261017-R5A-PC-LIVE-PROFILE-SQLITE-WEB' in report and 'status: BLOCKED\n' in report
manifest=blob(proof+'SHA256SUMS').decode().splitlines()
for line in manifest:
 expected,name=line.split('  ',1);assert hashlib.sha256(blob(proof+name)).hexdigest()==expected,name
bundle=json.loads(blob(proof+'BUNDLE.json'))['files'];raw={}
for item in bundle:
 b=blob(proof+item['path']);assert hashlib.sha256(b).hexdigest()==item['stored_sha256'];data=zlib.decompress(b) if item['zlib'] else b;assert hashlib.sha256(data).hexdigest()==item['raw_sha256'] and len(data)==item['raw_bytes'];raw[item['path']]=data
ns={'t':'http://microsoft.com/schemas/VisualStudio/TeamTest/2010'};passed=0
for name,data in raw.items():
 if name.startswith('tests/full-tests/'):
  count=E.fromstring(data).find('t:ResultSummary/t:Counters',ns);assert count is not None and int(count.attrib['failed'])==0;passed+=int(count.attrib['passed'])
assert passed==551
count=E.fromstring(raw['tests/service-final.trx.zlib']).find('t:ResultSummary/t:Counters',ns);assert count is not None and int(count.attrib['passed'])==201 and int(count.attrib['failed'])==0
wire=json.loads(raw['wire-mismatch.json']);assert sum(x['reservedSeverityFrames'] for x in wire)==2068 and sum(x['totalB3'] for x in wire)==2217
smoke=json.loads(raw['smoke02-summary.json']);assert smoke['result']=='PASS_OFFLINE' and smoke['quarantinedRows']==1 and smoke['noSerialAccess']
api=json.loads(raw['smoke02-api-live.json']);assert api[0]['observation']['rejectedB3Value']['severityGlobal']==240 and not api[0]['isCurrent']
provenance=json.loads(raw['source-provenance.json']);code=provenance['evaluated_code'];assert f'result_sha: {code}' in report
for path,expected in provenance['code_files'].items():assert hashlib.sha256(blob(path)).hexdigest()==expected,path
result=dict(result='PASS',mission_id='TR2-20261017-R5A-PC-LIVE-PROFILE-SQLITE-WEB',report_status='BLOCKED',sha=sha,evaluated_code=code,fully_compared_changed_files=len(names),manifest_files=len(manifest),bundle_files=len(bundle),full_suite_passed=551,final_service_passed=201,nonconforming_B3=2068,offline_http_and_quarantine_verified=True,utc=datetime.datetime.now(datetime.timezone.utc).isoformat());print(json.dumps(result,indent=2));Path(sys.argv[1]).write_text(json.dumps(result,indent=2)+'\n')
