from pathlib import Path
import sqlite3,json,shutil,hashlib,datetime,struct
p=Path('/home/lolo/dev/msm/reports/TR2-live-com4-loss-diag');d=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-LIVE-DEMO-20261019/data')
shutil.copyfile('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-DIAG-evidence/live-COM4-final.db',p/'live-final.db')
# Snapshot active logs as prefixes; original user logs are closed and remain byte-exact.
o=p/'pc-final-prefixes';o.mkdir(exist_ok=False);m=[]
for f in sorted(d.iterdir()):
 if not f.is_file() or f.name.startswith('live.db'):continue
 b=f.read_bytes();(o/f.name).write_bytes(b);m.append(dict(file=f.name,bytes=len(b),sha256=hashlib.sha256(b).hexdigest(),note='point-in-time prefix if final service still writing'))
(p/'pc-final-manifest.json').write_text(json.dumps(m,indent=2)+'\n')
for n in ['serial-5512-26404.jsonl','service-5512-26404.log','serial-cycle1.jsonl','serial-cycle2.jsonl']:
 assert (d/n).read_bytes()==(p/'pc-initial'/n).read_bytes()
with sqlite3.connect('file:'+str(p/'pc-initial/live.db')+'?mode=ro',uri=True) as old,sqlite3.connect('file:'+str(p/'live-final.db')+'?mode=ro',uri=True) as final:
 assert old.execute('pragma integrity_check').fetchone()[0]==final.execute('pragma integrity_check').fetchone()[0]=='ok'
 before=old.execute('select row_id,payload_json from live_observation order by row_id').fetchall();after=final.execute('select row_id,payload_json from live_observation order by row_id').fetchall();assert len(before)==6336 and len(after)==7713 and after[:len(before)]==before
 byid={json.loads(s)['ObservationId']:json.loads(s) for _,s in after};new=[json.loads(s) for _,s in after[len(before):]]
 for f in [p/'recovered-before-reproduction.json',p/'after-safe-audit-B0-B3.json',p/'final-after-audit-B0-B3.json']:
  api=json.loads(f.read_text());x=api['devices'][0];assert x['transportConnected'] and x['observation']['snapshot']['acquisitionStopped'];assert x['observation']['observationId'] in byid
 for f in (p/'physical').glob('restored-running-*.json'):
  x=json.loads(f.read_text());assert x['overview']['devices'][0]['isCurrent']
  for row in x['history']['entries']:
   a=row['observation'];b=byid[a['observationId']];assert a['receivedAt']==b['ReceivedAt']
   if b['Snapshot']:
    assert a['snapshot']['value']=={k[0].lower()+k[1:]:v for k,v in b['Snapshot']['Value'].items()}
commands=[]
for action,tx in [('start',18),('stop',19)]:
 rows=[json.loads(s) for s in (p/f'physical/restored-{action}.jsonl').read_text().splitlines()];terminal=next(x for x in rows if x.get('state')=='terminal_and_effect_observed');assert terminal['transaction']==tx and terminal['epoch']==5 and terminal['b5'][16:18]==[4,0];commands.append(terminal)
raw=(p/'safe-audit-final/fram.bin').read_bytes();before=(p/'hardware/fram-initial.bin').read_bytes();assert raw==(p/'model-final.bin').read_bytes() and raw[:122880]==before[:122880] and raw[245760:]==before[245760:]
finalapi=json.loads((p/'final-after-audit-B0-B3.json').read_text());audit=json.loads((p/'safe-audit-final/capture.json').read_text());assert datetime.datetime.fromisoformat(finalapi['devices'][0]['observation']['receivedAt'])>datetime.datetime.fromisoformat(audit['utc'])
refused=p/'refuse-active-service';assert not list(refused.glob('*.gdb')) and not list(refused.glob('*.bin')) and 'refused before any target connection' in (p/'refuse-active-service.log').read_text()
summary=dict(result='PASS_COM4_DIAG_AND_RECOVERY',cause=json.loads((p/'causality.json').read_text()),commands=commands,old_SQLite_rows_preserved=6336,final_SQLite_snapshot_rows=7713,new_SQLite_rows=1377,original_user_logs_unchanged=True,fram_final_sha256=hashlib.sha256(raw).hexdigest(),full_fram_exact_model=True,historical_and_tail_unchanged=True,journal=102,root=115,epoch=5,run=14,config=3,terminal='STOP5/19',new_firmware_flash=False,software_resets=2,Windows_binaries_unchanged=True,post_final_audit_real_B0_B3_received_without_reset=True,final_service_running=True,final_target_stopped_HELD=True,final_GDB_detached=True,new_measurements_current=sum(x['Snapshot'] is not None and x['Snapshot']['ValuesValid'] and x['Snapshot']['Fresh'] and not x['Snapshot']['Held'] for x in new),new_intentional_pause_lost=sum(x['Continuity']==6 for x in new),new_rejected=sum(x['Continuity']==7 for x in new),limit='bounded physical recovery, no new endurance/industrial qualification',utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
assert not summary['new_rejected']
(p/'summary.json').write_text(json.dumps(summary,indent=2)+'\n');print(json.dumps({k:v for k,v in summary.items() if k not in ['cause','commands']},indent=2))
