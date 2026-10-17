from pathlib import Path
import subprocess,json,hashlib,zlib,datetime,sys
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4U-live-runtime');initial='70bffe63048ebc5b169852ef9814389db59ea56f'
def git(*a):return subprocess.check_output(['git',*a],cwd=r)
sha=git('rev-parse','origin/main').decode().strip();assert sha==git('rev-parse','HEAD').decode().strip();remote=git('ls-remote','origin','refs/heads/main').decode().split()[0];assert remote==sha
names=git('diff','--name-only','-z',initial,sha).decode().strip('\0').split('\0');data={}
for n in names:
 raw=git('show',sha+':'+n);assert raw==(r/n).read_bytes(),n;data[n]=raw
base='Modbus RTU/00_gouvernance/Echanges_Codex/';proof=base+'preuves/TR2-20261017-R4U/'
report=data[base+'DERNIER_RAPPORT.md'].decode();assert 'mission_id: TR2-20261017-R4U-LIVE-RUNTIME-RTU-PHYSICAL' in report
s=json.loads(data[proof+'physical-final-summary.json']);assert s['result']=='PASS_BOUNDED_NONPRODUCTION' and s['bench']['xyz']==3045698 and s['serial']['adu_count']==1632 and s['full_fram_exact_metadata_model']
# Verify every proof from complete Git blob, and decode actual compressed records.
manifest=data[proof+'SHA256SUMS'].decode().splitlines()
for line in manifest:
 expected,name=line.split('  ',1);raw=data.get(proof+name)
 if raw is None:raw=git('show',sha+':'+proof+name)
 assert hashlib.sha256(raw).hexdigest()==expected,name
for rec in json.loads(data[proof+'BUNDLE.json'])['files']:
 stored=data.get(proof+rec['path'])
 if stored is None:stored=git('show',sha+':'+proof+rec['path'])
 raw=zlib.decompress(stored) if rec['zlib'] else stored;assert len(raw)==rec['raw_bytes'] and hashlib.sha256(raw).hexdigest()==rec['raw_sha256']
raw=zlib.decompress(data[proof+'hardware/live05-master.jsonl.zlib']);records=[json.loads(x) for x in raw.splitlines()];assert len([x for x in records if x['kind']=='adu'])==1632 and any(x['kind']=='summary' and x['passed'] for x in records)
rest=json.loads(data[proof+'hardware/restoration-summary.json']);assert rest['flash_exact_initial'] and rest['fram_exact_last_physical']
result=dict(result='PASS',mission_id='TR2-20261017-R4U-LIVE-RUNTIME-RTU-PHYSICAL',sha=sha,all_changed_files_fully_read_and_equal=len(names),evidence_manifest_files=len(manifest),bundle_files=len(json.loads(data[proof+'BUNDLE.json'])['files']),master_adus_parsed=1632,restoration_verified_from_remote=True,report_status='DONE' if 'status: DONE\n' in report else 'IN_PROGRESS',utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
print(json.dumps(result,indent=2));Path(sys.argv[1]).write_text(json.dumps(result,indent=2)+'\n')
