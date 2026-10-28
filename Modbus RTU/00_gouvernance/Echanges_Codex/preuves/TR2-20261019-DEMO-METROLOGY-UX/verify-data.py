from pathlib import Path
import json,sqlite3,csv,collections,hashlib
p=Path(__file__).parent
b=sqlite3.connect('file:/mnt/c/Users/lolo/AppData/Local/MSM/TR2-DEMO-METROLOGY-UX/baseline.db?mode=ro',uri=True);f=sqlite3.connect('file:'+str(p/'final.db')+'?mode=ro',uri=True);assert f.execute('pragma integrity_check').fetchone()[0]=='ok'
old=b.execute('SELECT * FROM live_observation ORDER BY row_id').fetchall();cols=[x[1] for x in f.execute('pragma table_info(live_observation)')];base_count=len(old)
assert f.execute('SELECT * FROM live_observation WHERE row_id<=? ORDER BY row_id',(base_count,)).fetchall()==old
payload={rid:json.loads(raw) for rid,raw in f.execute('SELECT row_id,payload_json FROM live_observation')}
fields=['rmsGlobalMg','peakGlobalMg','rmsXMg','rmsYMg','rmsZMg','peakXMg','peakYMg','peakZMg']
exports=[]
for name in ['running','logical-loss','final']:
 file=p/(name+'-export.csv');rows=list(csv.DictReader(file.open(encoding='utf-8-sig',newline='')))
 for row in rows:
  o=payload[int(row['row_id'])];s=o.get('Snapshot');v=s.get('Value') if s else None
  assert row['observation_id']==o['ObservationId'] and row['received_pc']==o['ReceivedAt']
  assert row['device_id_observed']==(str(o['DeviceId']['Value']) if o['DeviceId'] else '')
  for field in fields:
   key=field[0].upper()+field[1:];assert row[field]==(str(v[key]) if v else '')
  for field,key in [('values_valid_at_receipt','ValuesValid'),('fresh_at_receipt','Fresh'),('held','Held')]:
   assert row[field]==(str(s[key]).lower() if s else '')
  assert row['historical_non_current']=='true' and row['window_utc_unavailable_if_empty']==(s.get('WindowUtc') or '' if s else '')
 exports.append(dict(file=file.name,rows=len(rows),exact_against_snapshot=True))
# Independently compare API snapshots and DOM point attributes against persisted original observations.
api_checks=[]
for file in ['baseline-api.json','held-api.json','logical-loss-api.json','post-audit-api.json']:
 d=json.loads((p/file).read_text())['/api/v2/live/overview']['body']
 for view in d['devices']:
  o=view['observation'];saved=next(x for x in payload.values() if x['ObservationId']==o['observationId']);assert o['receivedAt']==saved['ReceivedAt']
  if o.get('snapshot'):
   for field in fields:assert o['snapshot']['value'][field]==saved['Snapshot']['Value'][field[0].upper()+field[1:]]
  else:assert saved['Snapshot'] is None
 api_checks.append(file)
new=[o for rid,o in payload.items() if rid>base_count];fresh=[o for o in new if o.get('Snapshot') and o['Snapshot']['ValuesValid'] and o['Snapshot']['Fresh'] and not o['Snapshot']['Held']];held=[o for o in new if o.get('Snapshot') and o['Snapshot']['Held']]
new_rms=[o['Snapshot']['Value']['RmsGlobalMg'] for o in fresh]
out=dict(snapshot_rows=len(payload),initial_rows_preserved_exact=base_count,snapshot_integrity='ok',snapshot_sha256=hashlib.sha256((p/'final.db').read_bytes()).hexdigest(),new_rows=len(new),new_fresh=len(fresh),new_held=len(held),new_snapshot_null=sum(o.get('Snapshot') is None for o in new),new_rms_global_range_mg=[min(new_rms),max(new_rms)],csv=exports,api_exact_against_sqlite=api_checks,note='First raw window seq1 equals actual controller B3; service released during START, therefore no manufactured seq1 SQLite entry. Service observations are later actual windows; API/CSV correspond exactly to their original rows.')
(p/'data-verification.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out,indent=2))
