from pathlib import Path
import json,re,struct,hashlib,math,sqlite3,shutil
P=Path('/home/lolo/dev/msm/reports/TR2-r5b2');H=P/'hardware';E=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-r5b2-evidence')
def sha(b):return hashlib.sha256(b).hexdigest()
def crc(a):
 v=65535
 for b in a:
  v^=b
  for _ in range(8):v=(v>>1)^40961 if v&1 else v>>1
 return v

def readjsonl(f):return [json.loads(x) for x in f.read_text(errors='replace').splitlines() if x.strip()]
for n in ['live.db','live.db-wal','live.db-shm','initial-serial.jsonl','running-serial.jsonl','held-serial.jsonl','smoke-serial.jsonl','smoke02-serial.jsonl']:
 if (E/n).exists():shutil.copyfile(E/n,P/n)
frames={};service=[];views={};adus=[]
for phase in ['initial','running','held']:
 frames[phase]=[];request=None;response=b'';qt=None
 def finish():
  if request is None:return
  assert crc(request)==crc(response)==0 and request[:2]==response[:2] and request[1]==3,(phase,request,response)
  addr=int.from_bytes(request[2:4],'big');n=int.from_bytes(request[4:6],'big');assert len(response)==n*2+5 and response[2]==n*2
  record=dict(phase=phase,utc=qt,request=request.hex(),response=response.hex(),address=addr,count=n,crc_ok=True)
  adus.append(record)
  if addr==3000:frames[phase].append(struct.unpack('>48H',response[3:-2]))
 for x in readjsonl(P/(phase+'-serial.jsonl')):
  b=bytes.fromhex(x['Hex'])
  if x['Direction']=='tx':finish();request=b;response=b'';qt=x['ReceivedAt']
  else:assert request is not None;response+=b
 finish()
 views[phase]=[json.loads(x) for x in (P/(phase+'-service.log')).read_text(errors='replace').splitlines() if x.startswith('{"Observation"')]
 # Exact 48-word response to decoded application observation, in the same poll order.
 snapshots=[x for x in views[phase] if x['Observation']['Snapshot'] is not None];assert len(snapshots)==len(frames[phase])
 for view,w in zip(snapshots,frames[phase]):
  s=view['Observation']['Snapshot'];v=s['Value'];expected={k:w[i] for k,i in [('StatusGlobal',0),('ValidityFlags',1),('AlarmFlags',2),('SeverityGlobal',3),('DominantAxis',30),('ExceedGlobal',31),('ExceedX',32),('ExceedY',33),('ExceedZ',34),('AlarmLatched',35)]}
  for k,i in [('LastUpdateTr2Seconds',4),('ValueAgeMilliseconds',6),('CalculationSequence',8),('WindowDurationMilliseconds',10),('ValidSampleCount',12),('RmsGlobalMg',14),('PeakGlobalMg',16),('RmsXMg',18),('RmsYMg',20),('RmsZMg',22),('PeakXMg',24),('PeakYMg',26),('PeakZMg',28),('ExceedCount',36),('AlarmCount',38)]:expected[k]=(w[i]<<16)|w[i+1]
  assert v==expected and not any(w[40:]) and w[3]<=4 and not w[1]&0xE100
  expectedsev=0 if w[1]&6!=6 else 4 if w[2]&15 else 3 if w[2]&240 else 2 if not w[1]&64 else 1
  assert w[3]==expectedsev and not w[1]&64 and w[4]==w[5]==0 and s['WindowUtc'] is None
  assert view['ArchiveState']=='Persisted' and view['Observation']['RunId'] is None and view['Observation']['Epoch'] is None and view['Observation']['ConfigurationId'] is None
  assert view['Observation']['Identity']['Profile']==2
 service+=views[phase]
# Byte-for-byte semantic correlation to each durable SQLite payload by unique observation id.
with sqlite3.connect(P/'live.db') as c:
 assert c.execute('pragma integrity_check').fetchone()[0]=='ok'
 archived=[json.loads(x[0]) for x in c.execute('select payload_json from live_observation order by row_id')]
 assert len({x['ObservationId'] for x in archived})==len(archived)
 byid={x['ObservationId']:x for x in archived}
 for view in service:assert byid[view['Observation']['ObservationId']]==view['Observation']
 real=[x for x in archived if ' COM4 ' in x['Source']];assert len(real)==len(service)
api_count=0;matched={}
for phase in ['initial','running','held']:
 matched[phase]=[]
 for row in readjsonl(P/(phase+'-http.jsonl')):
  if row['kind']!='http' or row['path']!='/api/v2/live':continue
  for v in json.loads(row['body']):
   o=v['observation'];assert o['observationId'] in byid
   a=byid[o['observationId']];assert o['receivedAt']==a['ReceivedAt'] and o['source']==a['Source']
   if a['Snapshot']:
    value=a['Snapshot']['Value'];wire=o['snapshot']['value']
    for k,num in value.items():assert wire[k[0].lower()+k[1:]]==num
   else:assert o['snapshot'] is None and not v['isCurrent']
   matched[phase].append(v);api_count+=1
running=[x for x in views['running'] if x['IsCurrent']];assert running
lost=[x for x in views['running'] if x['Observation']['Continuity']==6];assert lost and all(x['Observation']['Snapshot'] is None for x in lost)
assert len({x['Observation']['SegmentId'] for x in running})>=2
held=[x for x in views['held'] if x['Observation']['Snapshot']['Held']];assert held and all(not x['IsCurrent'] for x in held)
assert any(x['isCurrent'] for x in matched['running']) and any(x['observation']['snapshot'] and x['observation']['snapshot']['held'] for x in matched['held'])
dom=(P/'browser-running-dom.html').read_text();assert 'COM4' in dom and 'LiveDiagnostic' in dom and 'Valeur valide et fraîche reçue' in dom and 'mg)' in dom
commands=[];native=[]
for phase in ['start','stop']:
 rows=readjsonl(H/(phase+'-master.jsonl'));assert any(x['kind']=='summary' and x['passed'] for x in rows) and any(x['kind']=='closed' for x in rows)
 for x in rows:
  if x['kind']!='adu':continue
  q=bytes.fromhex(x['request']);a=bytes.fromhex(x['response']);assert crc(q)==crc(a)==0 and q[:2]==a[:2];native.append(x)
  if q[1]==16 and int.from_bytes(q[2:4],'big')==5000:commands.append(dict(code=int.from_bytes(q[7:9],'big'),tx=int.from_bytes(q[9:11],'big')))
assert commands==[dict(code=13,tx=12),dict(code=14,tx=13)]
text=(H/'live-phase.log').read_text(errors='replace');lines=text.splitlines()
def nums(s):return {k:int(v) for k,v in re.findall(r'(\w+) = ([0-9]+)(?=[,}])',s)}
def pick(s):return nums([x for x in lines if re.match(s,x)][-1])
u=pick(r'\$\d+ = \{stage = .*mode =');bench=pick(r'\$\d+ = \{stage = .*line =');work=pick(r'\$\d+ = \{context = .*work_limit_us');irq=pick(r'\$\d+ = \{uart_irq =');first=pick(r'\$\d+ = \{configuration =')
assert u['stage']==10 and u['error']==0 and u['starts']==1 and not bench['fifo_error'] and not bench['stop_error']
assert work['max_work_us']<=1000 and work['max_service_us']<=2500 and bench['max_gap_us']<=5000
assert not irq['events_dropped'] and not irq['error_callback'] and not irq['tx_fail_safe']
fram=(H/'live-fram.bin').read_bytes();before=(H/'fram-before.bin').read_bytes();assert fram==(P/'model-final.bin').read_bytes() and fram[:122880]==before[:122880] and fram[245760:]==before[245760:]
raw=list(struct.iter_unpack('<hhh',(H/'live-xyz.bin').read_bytes()));assert len(raw)==4096
conv=lambda n:(1 if n>=0 else -1)*((abs(n)*61+500)//1000);xyz=[tuple(conv(x) for x in row) for row in raw];sq=[sum(row[i]**2 for row in xyz) for i in range(3)];rms=[math.isqrt(sum(sq)//4096)]+[math.isqrt(x//4096) for x in sq];peak=[math.isqrt(max(sum(x*x for x in row) for row in xyz))]+[max(abs(row[i]) for row in xyz) for i in range(3)]
assert rms==[first['rms_global_mg'],first['rms_x_mg'],first['rms_y_mg'],first['rms_z_mg']] and peak==[first['peak_global_mg'],first['peak_x_mg'],first['peak_y_mg'],first['peak_z_mg']]
(P/'physical-service-adus.jsonl').write_text(''.join(json.dumps(x)+'\n' for x in adus))
example_api=next(x for x in matched['running'] if x['isCurrent']);oid=example_api['observation']['observationId']
example_view=next(x for x in views['running'] if x['Observation']['ObservationId']==oid)
snapshot_views=[x for x in views['running'] if x['Observation']['Snapshot'] is not None]
i=next(i for i,x in enumerate(snapshot_views) if x['Observation']['ObservationId']==oid)
raw_running=[x for x in adus if x['phase']=='running' and x['address']==3000]
received=re.search(r'Réception PC UTC : ([^<]+)',dom).group(1)
rendered=next(x for x in views['running'] if x['Observation']['ReceivedAt']==received)
(P/'correlated-example.json').write_text(json.dumps(dict(api=example_api,service=example_view,sqlite=byid[oid],raw_adu=raw_running[i],browser_rendered_observation=rendered,browser_dom='browser-running-dom.html',note='Same observationId raw ADU/service/SQLite/API; browser separately matched to exact receivedAt and stored values'),indent=2)+'\n')

summary=dict(result='PASS_BOUNDED_DIAGNOSTIC_END_TO_END',probe=u,bench=bench,work=work,irq=irq,commands=commands,service_ADUs=len(adus),native_ADUs=len(native),service_B3={k:len(v) for k,v in frames.items()},current_observations=len(running),held_observations=len(held),logical_lost_observations=len(lost),running_segments=len({x['Observation']['SegmentId'] for x in running}),sqlite_total=len(archived),sqlite_COM4=len(real),api_rows_correlated=api_count,all_raw_to_service_to_sqlite_API_correlated=True,service_FC10=0,firmware_budgets_pass=True,fram_sha256=sha(fram),historical_tail_unchanged=True,full_fram_exact_model=True,metadata=dict(journal=78,root=91,epoch=5,run=11,config_id=3,config_generation=3),distinct_rms_global=sorted({x['Observation']['Snapshot']['Value']['RmsGlobalMg'] for x in running}),first4096_oracle=dict(rms_mg=rms,peak_mg=peak,exact=True),browser='Headless Edge real DOM, not human browser observation',new_561_tests=False)
(P/'physical-summary.json').write_text(json.dumps(summary,indent=2)+'\n');print(json.dumps({k:v for k,v in summary.items() if k not in ['probe','bench','work','irq']},indent=2))
