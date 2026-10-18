from pathlib import Path
import subprocess,json,hashlib,zlib,datetime,sqlite3
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R5B2-physical-resume');p=Path('/home/lolo/dev/msm/reports/TR2-r5b2');base='dc47ee0e20dbbdd6047c517571c841eb5502d674';prefix='Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261018-R5B2/'
def git(*args):return subprocess.check_output(['git',*args],cwd=r)
subprocess.run(['git','fetch','origin','main'],cwd=r,check=True,capture_output=True);remote=git('rev-parse','origin/main').decode().strip();assert git('rev-parse','HEAD').decode().strip()==remote==git('ls-remote','origin','refs/heads/main').decode().split()[0]
def blob(f):return git('show',remote+':'+f)
files=git('diff','--name-only',base,remote).decode().splitlines()
for f in files:assert blob(f)==(r/f).read_bytes(),f
manifest=blob(prefix+'SHA256SUMS').decode().splitlines()
for x in manifest:
 h,f=x.split('  ',1);assert hashlib.sha256(blob(prefix+f)).hexdigest()==h,f
bundle=json.loads(blob(prefix+'BUNDLE.json'));raw={}
for x in bundle['artifacts']:
 b=blob(prefix+x['path']);assert hashlib.sha256(b).hexdigest()==x['stored_sha256'];a=zlib.decompress(b) if x['encoding']=='zlib' else b;assert len(a)==x['bytes'] and hashlib.sha256(a).hexdigest()==x['sha256'];raw[x['source']]=a
s=json.loads(raw['physical-summary.json']);assert s['current_observations']==88 and s['held_observations']==54 and s['logical_lost_observations']==28
assert raw['hardware/flash-before.bin']==raw['hardware/flash-restored.bin'] and hashlib.sha256(raw['hardware/flash-restored.bin']).hexdigest()=='c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26'
assert raw['hardware/live-fram.bin']==raw['model-final.bin']==raw['hardware/fram-after-restoration.bin'] and hashlib.sha256(raw['hardware/live-fram.bin']).hexdigest()=='2b501a835b9a02c43dc0738bd7af167b7d049fbffb4e9ef677ed2e867f47dd84'
a=raw['hardware/fram-before.bin'];b=raw['hardware/live-fram.bin'];assert a[:122880]==b[:122880] and a[245760:]==b[245760:]
def crc(a):
 v=65535
 for b in a:
  v^=b
  for _ in range(8):v=(v>>1)^40961 if v&1 else v>>1
 return v
adus=[json.loads(x) for x in raw['physical-service-adus.jsonl'].splitlines()];assert len(adus)==824
for x in adus:assert crc(bytes.fromhex(x['request']))==crc(bytes.fromhex(x['response']))==0 and bytes.fromhex(x['request'])[1]==3
native=0;fc10=0
for name in ['hardware/start-master.jsonl','hardware/stop-master.jsonl']:
 for row in raw[name].splitlines():
  x=json.loads(row)
  if x['kind']=='adu':
   q=bytes.fromhex(x['request']);a=bytes.fromhex(x['response']);assert crc(q)==crc(a)==0;native+=1;fc10+=int(q[1]==16)
assert native==1033 and fc10==4
f=p/('remote-db-'+remote[:7]+'.db');f.write_bytes(raw['live.db'])
with sqlite3.connect('file:'+str(f)+'?mode=ro',uri=True) as c:
 assert c.execute('pragma integrity_check').fetchone()[0]=='ok'
 rows=[json.loads(x[0]) for x in c.execute('select payload_json from live_observation')]
assert len(rows)==528 and len({x['ObservationId'] for x in rows})==528
assert len([x for x in rows if ' COM4 ' in x['Source']])==234
ex=json.loads(raw['correlated-example.json']);assert ex['api']['observation']['observationId']==ex['service']['Observation']['ObservationId']==ex['sqlite']['ObservationId']
assert 'Valeur valide et fraîche reçue' in raw['browser-running-dom.html'].decode() and 'COM4' in raw['browser-running-dom.html'].decode()
report=blob('Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md').decode();assert 'mission_id: TR2-20261018-R5B2-PHYSICAL-LIVE-RESUME' in report
status=report.split('status: ',1)[1].splitlines()[0]
v=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),remote_sha=remote,ls_remote_sha=remote,full_changed_files_compared=len(files),manifest_hashes_verified=len(manifest),bundle_artifacts_verified=len(bundle['artifacts']),physical_ADUs_verified=len(adus)+native,service_FC10=0,native_FC10=fc10,sqlite_rows=528,sqlite_COM4=234,api_example_exact=True,headless_DOM_verified=True,restored_full_Flash_exact=True,final_full_FRAM_exact_model_and_restoration=True,historical_tail_exact=True,report_status=status,independent_human_qualification=False)
(p/('readback-'+remote[:7]+'.json')).write_text(json.dumps(v,indent=2)+'\n');print(json.dumps(v,indent=2))
