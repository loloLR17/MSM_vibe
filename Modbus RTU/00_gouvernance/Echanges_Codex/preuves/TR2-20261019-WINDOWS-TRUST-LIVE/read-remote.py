from pathlib import Path
import subprocess,json,hashlib,zlib,sqlite3,datetime,sys
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-Windows-trust-live-resume');p=Path('/home/lolo/dev/msm/reports/TR2-windows-trust-live-resume');prefix='Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261019-WINDOWS-TRUST-LIVE/'
def git(*a):return subprocess.check_output(['git',*a],cwd=r)
subprocess.run(['git','fetch','origin','main'],cwd=r,check=True)
sha=git('rev-parse','origin/main').decode().strip();remote=git('ls-remote','origin','refs/heads/main').decode().split()[0];assert sha==remote==git('rev-parse','HEAD').decode().strip()
files=git('diff','--name-only','-z','cb34b3f1dd19cc04be0235cd568f61fb343e850b',sha).decode().strip('\0').split('\0')
for f in files:assert git('show',sha+':'+f)==(r/f).read_bytes(),f
manifest=json.loads(git('show',sha+':'+prefix+'BUNDLE.json'))
for item in manifest:
 b=git('show',sha+':'+prefix+item['stored']);assert hashlib.sha256(b).hexdigest()==item['stored_sha256'];raw=zlib.decompress(b) if item['compression'] else b;assert len(raw)==item['bytes'] and hashlib.sha256(raw).hexdigest()==item['sha256']
 if item['source']=='live-final.db':
  db=p/('remote-'+sha[:8]+'.db');assert not db.exists();db.write_bytes(raw)
  with sqlite3.connect('file:'+str(db)+'?mode=ro',uri=True) as conn:
   integrity=conn.execute('pragma integrity_check').fetchone()[0];rows=conn.execute('select count(*) from live_observation').fetchone()[0];assert integrity=='ok' and rows==6060
report=git('show',sha+':Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md').decode();assert 'mission_id: TR2-20261019-WINDOWS-TRUST-LIVE-RESUME' in report and '710.709s' in report
out=dict(result='PASS',sha=sha,changed_deliverables_reread_byte_exact=len(files),raw_and_stored_evidence_verified=len(manifest),remote_SQLite_integrity=integrity,remote_SQLite_rows=rows,report_status='DONE' if 'status: DONE' in report else 'IN_PROGRESS',utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),physical_PC_and_firmware_source='c36c9718a75e6b40c45707a482e8db836296cab2')
(p/('remote-readback-'+sha[:8]+'.json')).write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out,indent=2))
