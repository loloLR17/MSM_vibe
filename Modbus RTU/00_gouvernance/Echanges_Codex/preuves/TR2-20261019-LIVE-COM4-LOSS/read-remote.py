from pathlib import Path
import subprocess,json,hashlib,zlib,sqlite3,datetime,sys
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-LIVE-COM4-loss-diag');p=Path('/home/lolo/dev/msm/reports/TR2-live-com4-loss-diag');prefix='Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261019-LIVE-COM4-LOSS/'
def git(*a):return subprocess.check_output(['git',*a],cwd=r)
subprocess.run(['git','fetch','origin','main'],cwd=r,check=True)
sha=git('rev-parse','origin/main').decode().strip();remote=git('ls-remote','origin','refs/heads/main').decode().split()[0];assert sha==remote==git('rev-parse','HEAD').decode().strip()
files=git('diff','--name-only','-z','dfe5539eb1f9e4bbc3d850cab3dc84ac018bc925',sha).decode().strip('\0').split('\0')
for f in files:assert git('show',sha+':'+f)==(r/f).read_bytes(),f
manifest=json.loads(git('show',sha+':'+prefix+'BUNDLE.json'))
for item in manifest:
 b=git('show',sha+':'+prefix+item['stored']);assert hashlib.sha256(b).hexdigest()==item['stored_sha256'];raw=zlib.decompress(b) if item['compression'] else b;assert len(raw)==item['bytes'] and hashlib.sha256(raw).hexdigest()==item['sha256']
 if item['source']=='live-upgraded-final.db':
  db=p/('remote-upgraded-'+sha[:8]+'.db');assert not db.exists();db.write_bytes(raw)
  with sqlite3.connect('file:'+str(db)+'?mode=ro',uri=True) as conn:
   integrity=conn.execute('pragma integrity_check').fetchone()[0];rows=conn.execute('select count(*) from live_observation').fetchone()[0];assert integrity=='ok' and rows==13623
report=git('show',sha+':Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md').decode();assert 'mission_id: TR2-20261019-LIVE-COM4-LOSS-DIAG' in report and '846632' in report
out=dict(result='PASS',sha=sha,changed_deliverables_reread_byte_exact=len(files),raw_and_stored_evidence_verified=len(manifest),remote_SQLite_integrity=integrity,remote_SQLite_rows=rows,report_status='DONE' if 'status: DONE' in report else 'IN_PROGRESS',utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),physical_PC_source='d67741f2 (build b031d01a plus preserved patch)',firmware_source='c36c9718a75e6b40c45707a482e8db836296cab2')
(p/('remote-readback-'+sha[:8]+'.json')).write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out,indent=2))
