from pathlib import Path
import subprocess,json,hashlib,zlib,datetime,sys,xml.etree.ElementTree as E
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R5AFIX-b3-wire');p=Path('/home/lolo/dev/msm/reports/TR2-r5afix');initial=json.loads((p/'initial-state.json').read_text())['head']
def git(*a):return subprocess.check_output(['git',*a],cwd=r)
sha=git('rev-parse','origin/main').decode().strip();assert sha==git('rev-parse','HEAD').decode().strip()==git('ls-remote','origin','refs/heads/main').decode().split()[0]
names=git('diff','--name-only','-z',initial,sha).decode().strip('\0').split('\0');data={}
for n in names:
 raw=git('show',sha+':'+n);assert raw==(r/n).read_bytes(),n;data[n]=raw
def blob(n):return data[n] if n in data else git('show',sha+':'+n)
base='Modbus RTU/00_gouvernance/Echanges_Codex/';proof=base+'preuves/TR2-20261017-R5AFIX/';report=blob(base+'DERNIER_RAPPORT.md').decode();assert 'mission_id: TR2-20261017-R5AFIX-B3-WIRE-ORDER' in report
manifest=blob(proof+'SHA256SUMS').decode().splitlines()
for line in manifest:
 expected,name=line.split('  ',1);assert hashlib.sha256(blob(proof+name)).hexdigest()==expected,name
bundle=json.loads(blob(proof+'BUNDLE.json'))['files']
for item in bundle:
 stored=blob(proof+item['path']);assert hashlib.sha256(stored).hexdigest()==item['stored_sha256'];raw=zlib.decompress(stored) if item['zlib'] else stored;assert len(raw)==item['raw_bytes'] and hashlib.sha256(raw).hexdigest()==item['raw_sha256']
v=json.loads(blob(proof+'physical-summary.json'));assert v['result']=='PASS_BOUNDED_NONPRODUCTION' and v['aduCount']==1095 and v['B3Count']==563 and v['validFreshCount']==462 and v['heldCount']==51
assert v['firstValid']['alarms3002']==240 and v['firstValid']['severity3003']==3 and v['lastHeld']['severity3003']==0 and v['full_fram_exact_model']
rows=[json.loads(x) for x in zlib.decompress(blob(proof+'hardware/fix-master.jsonl.zlib')).splitlines()];assert sum(x['kind']=='adu' for x in rows)==1095 and any(x['kind']=='summary' and x['passed'] for x in rows)
rest=json.loads(blob(proof+'hardware/restoration-summary.json'));assert rest['pages']==19 and rest['flash_exact_initial'] and rest['fram_exact_last_physical']
assert zlib.decompress(blob(proof+'hardware/flash-restored.bin.zlib'))==zlib.decompress(blob(proof+'hardware/flash-before.bin.zlib'))
assert zlib.decompress(blob(proof+'hardware/fram-after-restoration.bin.zlib'))==zlib.decompress(blob(proof+'hardware/fix-fram.bin.zlib'))==zlib.decompress(blob(proof+'model-final.bin.zlib'))
ns={'t':'http://microsoft.com/schemas/VisualStudio/TeamTest/2010'}
for file,count in [('service.trx.zlib',202),('physical-replay.trx.zlib',2)]:
 c=E.fromstring(zlib.decompress(blob(proof+file))).find('t:ResultSummary/t:Counters',ns);assert c is not None and int(c.attrib['passed'])==count and int(c.attrib['failed'])==0
result=dict(result='PASS',mission_id='TR2-20261017-R5AFIX-B3-WIRE-ORDER',sha=sha,report_status='DONE' if 'status: DONE\n' in report else 'IN_PROGRESS',all_changed_files_fully_compared=len(names),manifest_files=len(manifest),bundle_files=len(bundle),physical_ADUs_parsed=1095,physical_B3=563,PC_SQLite_replay_test_PASS=True,full_flash_and_fram_restoration_verified=True,utc=datetime.datetime.now(datetime.timezone.utc).isoformat());print(json.dumps(result,indent=2));Path(sys.argv[1]).write_text(json.dumps(result,indent=2)+'\n')
