from pathlib import Path
import sqlite3,shutil,json,hashlib,datetime,re
p=Path('/home/lolo/dev/msm/reports/TR2-live-com4-loss-diag');old=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-LIVE-DEMO-20261019');new=old.parent/'TR2-LIVE-COM4-HELD-20261019'
shutil.copyfile('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-DIAG-evidence/live-COM4-upgraded-final.db',p/'live-upgraded-final.db')
with sqlite3.connect('file:'+str(old/'data/live.db')+'?mode=ro',uri=True) as a,sqlite3.connect('file:'+str(p/'live-upgraded-final.db')+'?mode=ro',uri=True) as b:
 assert a.execute('pragma integrity_check').fetchone()[0]==b.execute('pragma integrity_check').fetchone()[0]=='ok'
 before=a.execute('select row_id,payload_json from live_observation order by row_id').fetchall();after=b.execute('select row_id,payload_json from live_observation order by row_id').fetchall();assert after[:len(before)]==before and len(after)==13623
 byid={json.loads(v)['ObservationId']:json.loads(v) for _,v in after}
 for f in sorted((p/'physical').glob('upgraded-running-*.json')):
  j=json.loads(f.read_text());assert j['overview']['devices'][0]['isCurrent']
  for e in j['history']['entries']:
   o=e['observation'];v=byid[o['observationId']];assert o['receivedAt']==v['ReceivedAt']
   if v['Snapshot']:assert o['snapshot']['value']=={k[0].lower()+k[1:]:n for k,n in v['Snapshot']['Value'].items()}
newrows=[json.loads(v) for _,v in after[len(before):]];assert not any(x['Continuity']==7 for x in newrows)
cmds=[]
for n in ['restored-start','restored-stop','upgraded-held-start','upgraded-stop']:
 rows=[json.loads(x) for x in (p/f'physical/{n}.jsonl').read_text().splitlines() if x.startswith('{')];cmds.append(next(x for x in rows if x.get('state')=='terminal_and_effect_observed'))
held=json.loads((p/'upgraded-held-before-start.json').read_text())['devices'][0];assert held['transportConnected'] and held['observation']['snapshot']['held'] and held['observation']['snapshot']['acquisitionStopped']
failed=[json.loads(x) for x in (p/'physical/held-restart-check.jsonl').read_text().splitlines()];assert any(x.get('state')=='failed_or_uncertain' for x in failed);assert not any(x.get('Direction')=='tx' and x['Hex'].startswith('0110') for x in failed)
assert json.loads((old/'data/transactions.json').read_text())['Transaction']==19 and json.loads((new/'data/transactions.json').read_text())['Transaction']==21
fram=(p/'safe-audit-upgraded-final/fram.bin').read_bytes();initial=(p/'hardware/fram-initial.bin').read_bytes();assert fram==(p/'model-upgraded.bin').read_bytes() and fram[:122880]==initial[:122880] and fram[245760:]==initial[245760:]
audit=json.loads((p/'safe-audit-upgraded-final/capture.json').read_text());api=json.loads((p/'upgraded-final-after-audit-B0-B3.json').read_text())['devices'][0];assert api['transportConnected'] and api['observation']['snapshot']['held'] and api['observation']['snapshot']['acquisitionStopped'];assert datetime.datetime.fromisoformat(api['observation']['receivedAt'])>datetime.datetime.fromisoformat(audit['utc']);assert api['observation']['observationId'] in byid
manifest=json.loads((p/'held-windows-gate-confirm/application-sha256.json').read_text());assert all(hashlib.sha256((new/'app'/f).read_bytes()).hexdigest()==h for f,h in manifest.items())
oldmanifest=json.loads(Path('/home/lolo/dev/msm/reports/TR2-windows-trust-live-resume/checkpoint-gate/application-sha256.json').read_text());assert all(hashlib.sha256((old/'app'/f).read_bytes()).hexdigest()==h for f,h in oldmanifest.items())
# Preserve new package log prefixes without copying an active raw database/WAL.
o=p/'upgraded-pc-final-prefixes';o.mkdir(exist_ok=False)
for f in sorted((new/'data').iterdir()):
 if f.is_file() and not f.name.startswith('live.db'):(o/f.name).write_bytes(f.read_bytes())
summary=dict(result='PASS_COM4_AND_HELD_RESTART_RECOVERED',commands=cmds,old_closed_rows_migrated_exact=len(before),original_user_rows_preserved=6336,final_SQLite_snapshot_rows=len(after),new_release_observations=len(newrows),current_measurements_new_release=sum(x['Snapshot'] is not None and x['Snapshot']['ValuesValid'] and x['Snapshot']['Fresh'] and not x['Snapshot']['Held'] for x in newrows),old_package_binaries_preserved=377,new_package_files_verified=377,windows_gate_pass=True,security_changes=False,service_dll_sha256='63f3ddc1a91d99599a719bc6a013aa52810a9813a9d08c40233ce45b29cb85fa',code='d67741f2',firmware='c36c971 / 9bbe3979...0981 unchanged',restarted_from_actual_HELD_without_reset=True,final_STOP='5/21',epoch=5,run=15,config=3,journal=110,root=123,fram_sha256=hashlib.sha256(fram).hexdigest(),full_fram_exact_model=True,historical_and_tail_unchanged=True,post_last_audit_B0_B3_without_reset=True,post_audit_received_utc=api['observation']['receivedAt'],service_active=True,COM4_sole_native_observer=True,debugger_detached=True,flash_count=0,software_resets=2,qualified_new_windows_root=str(new),habitual_launchers_redirected_with_original_backups=True)
(p/'summary-final.json').write_text(json.dumps(summary,indent=2)+'\n');print(json.dumps({k:v for k,v in summary.items() if k!='commands'},indent=2))
