from pathlib import Path
import json,struct,math,sqlite3,re,hashlib,datetime,csv,io
P=Path('/home/lolo/dev/msm/reports/TR2-windows-trust-live-resume');H=P/'hardware';D=P/'physical'
def read(f):return [json.loads(x) for x in f.read_text(errors='replace').splitlines() if x.startswith('{')]
def crc(a):
 v=65535
 for b in a:
  v^=b
  for _ in range(8):v=(v>>1)^40961 if v&1 else v>>1
 return v

def frames(f):
 result=[];q=None;a=b'';qt=None;last=None
 def finish():
  if q is None:return
  assert crc(q)==crc(a)==0 and q[:2]==a[:2],(f,q,a)
  address=int.from_bytes(q[2:4],'big');count=int.from_bytes(q[4:6],'big')
  assert (len(a)==count*2+5 and a[2]==count*2) if q[1]==3 else (q[1]==16 and len(a)==8 and a[:6]==q[:6]),(f,q,a)
  result.append(dict(utc=qt,end_utc=last,request=q.hex(),response=a.hex(),fc=q[1],address=address,count=count))
 for x in read(f):
  if 'Hex' not in x:continue
  b=bytes.fromhex(x['Hex'])
  if x['Direction']=='tx':finish();q=b;a=b'';qt=x['ReceivedAt']
  else:assert q is not None;a+=b;last=x['ReceivedAt']
 finish();return result

def expected(w):
 out={k:w[i] for k,i in [('StatusGlobal',0),('ValidityFlags',1),('AlarmFlags',2),('SeverityGlobal',3),('DominantAxis',30),('ExceedGlobal',31),('ExceedX',32),('ExceedY',33),('ExceedZ',34),('AlarmLatched',35)]}
 for k,i in [('LastUpdateTr2Seconds',4),('ValueAgeMilliseconds',6),('CalculationSequence',8),('WindowDurationMilliseconds',10),('ValidSampleCount',12),('RmsGlobalMg',14),('PeakGlobalMg',16),('RmsXMg',18),('RmsYMg',20),('RmsZMg',22),('PeakXMg',24),('PeakYMg',26),('PeakZMg',28),('ExceedCount',36),('AlarmCount',38)]:out[k]=(w[i]<<16)|w[i+1]
 return out
service=[];raw=[];b3counts={};handoffs=[];cmds=[]
for cycle in ['cycle1','cycle2']:
 rows=[x for x in read(D/('service-'+cycle+'.log')) if 'Observation' in x];service+=rows
 sf=frames(P/('serial-'+cycle+'.jsonl'));raw+=sf;assert all(x['fc']==3 for x in sf)
 b3=[struct.unpack('>48H',bytes.fromhex(x['response'])[3:-2]) for x in sf if x['address']==3000]
 obs=[x for x in rows if x['Observation']['Snapshot'] is not None];assert len(obs)==len(b3),(cycle,len(obs),len(b3))
 b3counts[cycle]=len(b3)
 for view,w in zip(obs,b3):
  assert view['Observation']['Snapshot']['Value']==expected(w)
  assert view['ArchiveState']=='Persisted' and not view['ArchiveError'] and not any(w[40:])
 for action in ['start','stop']:
  name=cycle+'-'+action;rr=read(D/(name+'.jsonl'));ff=frames(D/(name+'.jsonl'))
  assert json.loads((D/(name+'-exit.json')).read_text())['exit']==0
  ack=next(x for x in rr if x.get('state')=='poller_paused_handle_released');terminal=next(x for x in rr if x.get('state')=='terminal_and_effect_observed')
  writes=[x for x in ff if x['fc']==16];assert len(writes)==2 and [x['address'] for x in writes]==[5020,5000]
  assert ack['utc']<ff[0]['utc'] and writes[0]['utc']>=ff[2]['end_utc']
  # Only the operator is transmitting between its first and last ADU.
  assert not any(x['utc']<=ff[-1]['end_utc'] and x['end_utc']>=ff[0]['utc'] for x in sf)
  handoffs.append(dict(command=name,ack_utc=ack['utc'],first_operator_tx=ff[0]['utc'],last_operator_rx=ff[-1]['end_utc'],service_ADUs_overlap=0,exclusive_open_succeeded_before_first_tx=True))
  cmds.append(terminal)
# Closed Windows database is copied byte-for-byte only after service shutdown.
with sqlite3.connect('file:'+str(P/'live-final.db')+'?mode=ro',uri=True) as db:
 assert db.execute('pragma integrity_check').fetchone()[0]=='ok'
 records=[json.loads(x[0]) for x in db.execute('select payload_json from live_observation order by row_id')]
 byid={x['ObservationId']:x for x in records};assert len(byid)==len(records)==len(service)
 for v in service:assert byid[v['Observation']['ObservationId']]==v['Observation']
current=[v for v in service if v['IsCurrent']];held=[v for v in service if v['Observation']['Snapshot'] and v['Observation']['Snapshot']['Held']]
lost=[v for v in service if v['Observation']['Continuity']==6];assert all(x['Observation']['Snapshot'] is None and 'Operator logical disconnect' in x['Observation']['Detail'] for x in lost)
assert not any(v['Observation']['Continuity']==7 for v in service)
# HTTP fetch failures during the intentional stopped-host audit interval are not COM failures.
fetch_errors=read(D/'monitor-errors.jsonl') if (D/'monitor-errors.jsonl').exists() else []
parse=lambda x:datetime.datetime.fromisoformat(x.replace('Z','+00:00'))
off=parse(json.loads((D/'service-cycle1-exit.json').read_text())['utc'])
second_first=min(parse(v['Observation']['ReceivedAt']) for v in read(D/'service-cycle2.log') if 'Observation' in v)
assert all(off<=parse(x['utc'])<second_first for x in fetch_errors),fetch_errors
monitor=read(D/'monitor.jsonl');assert all(x['http']==200 and x['stored'] is not None and x['device']['archiveState']=='Persisted' for x in monitor)
for x in monitor:
 o=x['device']['observation'];a=byid[o['observationId']];assert json.loads(x['stored']['payload_json'])==a and o['receivedAt']==a['ReceivedAt']
 if a['Snapshot']:
  for k,v in a['Snapshot']['Value'].items():assert o['snapshot']['value'][k[0].lower()+k[1:]]==v
 for k in ['RunId','Epoch','ConfigurationId','ConfigurationGeneration','EventMonotonicMicroseconds']:assert a[k] is None
csv_rows=0
for f in D.glob('*-export.csv'):
 rows=list(csv.DictReader(io.StringIO(f.read_text())))
 assert len(rows)<=500
 for row in rows:
  a=byid[row['observation_id']];sn=a['Snapshot'];assert row['received_pc']==a['ReceivedAt'] and row['historical_non_current']=='true'
  assert not row['window_utc_unavailable_if_empty'] and not row['run_id'] and not row['epoch'] and not row['configuration_id']
  for key in ['rmsGlobalMg','peakGlobalMg','rmsXMg','rmsYMg','rmsZMg','peakXMg','peakYMg','peakZMg']:
   assert row[key]==(str(sn['Value'][key[0].upper()+key[1:]]) if sn else '')
  csv_rows+=1
assert (H/'cycle1-audit-fram.bin').read_bytes()==(P/'model-cycle1.bin').read_bytes()
assert (H/'cycle2-audit-fram.bin').read_bytes()==(P/'model-final.bin').read_bytes()
browser=[]
for f in sorted(D.glob('*-running-*.json'))+sorted(D.glob('*-held-*.json')):
 j=json.loads(f.read_text());assert j['history']['historical'] and not j['history']['isCurrent']
 for entry in j['history']['entries']:
  o=entry['observation'];a=byid[o['observationId']];assert o['receivedAt']==a['ReceivedAt']
  if a['Snapshot']:
   for k,v in a['Snapshot']['Value'].items():assert o['snapshot']['value'][k[0].lower()+k[1:]]==v
 browser.append(dict(file=f.name,utc=j['utc'],globalPoints=j['dom']['globalPoints'],xyzPoints=j['dom']['xyzPoints'],current=j['overview']['devices'][0]['isCurrent'],entries=len(j['history']['entries'])))
assert all(next(x for x in browser if x['file']==f'cycle1-running-{k}.json')['current'] for k in range(2))
a=next(x for x in browser if x['file']=='cycle1-running-0.json');b=next(x for x in browser if x['file']=='cycle1-running-1.json');assert b['globalPoints']>a['globalPoints'] and b['xyzPoints']>a['xyzPoints']
# Exact first physical window independent integer oracle, no metrological calibration claim.
oracles=[]
for cycle in ['cycle1','cycle2']:
 text=(H/(cycle+'-audit.log')).read_text(errors='replace')
 line=next(x for x in text.splitlines() if re.match(r'\$\d+ = \{configuration = ',x));nums={k:int(v) for k,v in re.findall(r'(\w+) = ([0-9]+)(?=[,}])',line)}
 rawxyz=list(struct.iter_unpack('<hhh',(H/(cycle+'-audit-xyz.bin')).read_bytes()));assert len(rawxyz)==4096
 conv=lambda n:(1 if n>=0 else -1)*((abs(n)*61+500)//1000)
 xyz=[tuple(conv(x) for x in row) for row in rawxyz];sq=[sum(row[i]**2 for row in xyz) for i in range(3)]
 rms=[math.isqrt(sum(sq)//4096)]+[math.isqrt(x//4096) for x in sq];peak=[math.isqrt(max(sum(x*x for x in row) for row in xyz))]+[max(abs(row[i]) for row in xyz) for i in range(3)]
 assert rms==[nums[k] for k in ['rms_global_mg','rms_x_mg','rms_y_mg','rms_z_mg']] and peak==[nums[k] for k in ['peak_global_mg','peak_x_mg','peak_y_mg','peak_z_mg']]
 oracles.append(dict(cycle=cycle,rms_mg=rms,peak_mg=peak,first4096_exact=True))
start=next(x for x in cmds if x['command']=='start' and x['transaction']==16);stop=next(x for x in cmds if x['command']=='stop' and x['transaction']==17)
# Conservative lower bound RUNNING: terminal START to explicit STOP request, excludes START pipeline.
request=next(x for x in read(D/'cycle2-stop.jsonl') if x.get('state')=='requested_not_terminal')
dt=lambda x:datetime.datetime.fromisoformat(x.replace('Z','+00:00'))
duration=(dt(request['utc'])-dt(start['utc'])).total_seconds();assert duration>600
summary=dict(result='PASS_LIMITED_C36C971_PHYSICAL',pc_and_firmware_source='c36c9718a75e6b40c45707a482e8db836296cab2',code_fix_source='8a54a289dae2dad312cf2245c0db03fc76cc67ef',new_fix_deployment_blocked_deferred=True,security_modified=False,service_FC10=0,raw_service_ADUs=len(raw),B3_counts=b3counts,SQLite_observations=len(records),current_observations=len(current),held_observations=len(held),intentional_pause_lost_observations=len(lost),unintentional_transport_errors=0,API_monitor_snapshots_correlated=len(monitor),all_raw_B3_service_SQLite_API_correlated=True,HTTP_failures_during_intentional_stopped_host_interval=len(fetch_errors),commands=cmds,handoffs=handoffs,browser=browser,first4096_oracles=oracles,CSV_rows_correlated=csv_rows,full_FRAM_exact_to_genuine_core_metadata_model=True,second_run_conservative_running_seconds=duration,firmware_has_no_duration_cutoff=True,final_firmware='LIVE retained',final_command='STOP5/17 terminal+HELD',metrology_or_industrial_qualification=False)
(P/'physical-summary.json').write_text(json.dumps(summary,indent=2)+'\n');(P/'service-ADUs.jsonl').write_text(''.join(json.dumps(x)+'\n' for x in raw));print(json.dumps({k:v for k,v in summary.items() if k not in ['commands','browser','handoffs']},indent=2))
