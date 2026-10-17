from pathlib import Path
import subprocess,json,hashlib,zlib,datetime,sys
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4V-rtu-latency');initial='8010d0101d70170a13731ca928833ce8c95774c3'
def git(*a):return subprocess.check_output(['git',*a],cwd=r)
sha=git('rev-parse','origin/main').decode().strip();assert sha==git('rev-parse','HEAD').decode().strip();assert git('ls-remote','origin','refs/heads/main').decode().split()[0]==sha
names=git('diff','--name-only','-z',initial,sha).decode().strip('\0').split('\0');data={}
for n in names:
 raw=git('show',sha+':'+n);assert raw==(r/n).read_bytes(),n;data[n]=raw
def blob(n):return data[n] if n in data else git('show',sha+':'+n)
base='Modbus RTU/00_gouvernance/Echanges_Codex/';proof=base+'preuves/TR2-20261017-R4V/';report=data[base+'DERNIER_RAPPORT.md'].decode();assert 'mission_id: TR2-20261017-R4V-RTU-START-STOP-LATENCY' in report and 'result_sha: 8db2f673ac46c3e4d2bb64f0bbd39ca16bd39a4e' in report
manifest=blob(proof+'SHA256SUMS').decode().splitlines()
for line in manifest:
 expected,name=line.split('  ',1);assert hashlib.sha256(blob(proof+name)).hexdigest()==expected,name
bundle=json.loads(blob(proof+'BUNDLE.json'))['files']
for rec in bundle:
 stored=blob(proof+rec['path']);assert hashlib.sha256(stored).hexdigest()==rec['stored_sha256'];raw=zlib.decompress(stored) if rec['zlib'] else stored;assert len(raw)==rec['raw_bytes'] and hashlib.sha256(raw).hexdigest()==rec['raw_sha256']
counts={}
for mode,count in [('baseline',1655),('paced',1075)]:
 records=[json.loads(x) for x in zlib.decompress(blob(proof+'hardware/'+mode+'-master.jsonl.zlib')).splitlines()];assert sum(x['kind']=='adu' for x in records)==count and any(x['kind']=='summary' and x['passed'] for x in records);counts[mode]=count
 s=json.loads(blob(proof+mode+'-summary.json'));assert s['result']=='PASS_BOUNDED' and s['full_fram_exact_model']
rest=json.loads(blob(proof+'hardware/restoration-summary.json'));assert rest['pages']==19 and rest['flash_exact_initial'] and rest['fram_exact_last_physical']
assert zlib.decompress(blob(proof+'hardware/flash-restored.bin.zlib'))==zlib.decompress(blob(proof+'hardware/flash-before.bin.zlib'))
assert zlib.decompress(blob(proof+'hardware/fram-after-restoration.bin.zlib'))==zlib.decompress(blob(proof+'model-paced.bin.zlib'))
result=dict(result='PASS',mission_id='TR2-20261017-R4V-RTU-START-STOP-LATENCY',sha=sha,all_changed_files_fully_read_and_equal=len(names),evidence_manifest_files=len(manifest),bundle_files=len(bundle),master_adus_parsed=counts,restoration_verified_from_remote=True,report_status='DONE' if 'status: DONE\n' in report else 'IN_PROGRESS',utc=datetime.datetime.now(datetime.timezone.utc).isoformat());print(json.dumps(result,indent=2));Path(sys.argv[1]).write_text(json.dumps(result,indent=2)+'\n')
