from pathlib import Path
import json,re,struct,math,hashlib,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-r4u');h=p/'hardware'
def numbers(line):return {k:int(v) for k,v in re.findall(r'(\w+) = ([0-9]+)(?=[,}])',line)}
def select(lines,pattern):return [x for x in lines if re.match(pattern,x)][-1]
def crc(data):
 v=65535
 for b in data:
  v^=b
  for _ in range(8):v=(v>>1)^40961 if v&1 else v>>1
 return v
log=(h/'live05-phase.log').read_text(errors='replace');lines=log.splitlines();u=numbers(select(lines,r'\$\d+ = \{stage = .*mode ='));b=numbers(select(lines,r'\$\d+ = \{stage = .*line ='));w=numbers(select(lines,r'\$\d+ = \{context = .*work_limit_us'));clock=numbers(select(lines,r'\$\d+ = \{fifo ='));irq=numbers(select(lines,r'\$\d+ = \{uart_irq ='))
first=numbers(select(lines,r'\$\d+ = \{configuration ='));xyz=list(struct.iter_unpack('<hhh',(h/'live05-first-xyz.bin').read_bytes()));assert len(xyz)==4096
convert=lambda v:(1 if v>=0 else -1)*((abs(v)*61+500)//1000)
x=[tuple(convert(v) for v in row) for row in xyz];sq=[sum(row[j]**2 for row in x) for j in range(3)];rms=[math.isqrt(sum(sq)//4096)]+[math.isqrt(n//4096) for n in sq];peak=[math.isqrt(max(sum(v*v for v in row) for row in x))]+[max(abs(row[j]) for row in x) for j in range(3)]
assert rms==[first['rms_global_mg'],first['rms_x_mg'],first['rms_y_mg'],first['rms_z_mg']] and peak==[first['peak_global_mg'],first['peak_x_mg'],first['peak_y_mg'],first['peak_z_mg']]
raw=[json.loads(l) for l in (h/'live05-master.jsonl').read_text(errors='replace').splitlines()];adus=[x for x in raw if x['kind']=='adu'];summary=[x for x in raw if x['kind']=='summary'][-1];assert summary['passed']
lat=[];counts={};valid=[];held=[];before=[];frames=[]
for row in adus:
 q=bytes.fromhex(row['request']);a=bytes.fromhex(row['response']);assert crc(q)==crc(a)==0 and q[0]==a[0]==1 and q[1]==a[1]
 addr=int.from_bytes(q[2:4],'big');counts[str((q[1],addr))]=counts.get(str((q[1],addr)),0)+1;lat.append(row['elapsed_ms'])
 if a[1]==3:
  assert len(a)==a[2]+5 and a[2]==2*int.from_bytes(q[4:6],'big');v=struct.unpack('>'+str(a[2]//2)+'H',a[3:-2])
  if addr==3000:
   assert not v[1]&64 and v[4]==v[5]==0
   item=dict(utc=row['utc'],status=v[0],flags=v[1],sequence=v[8]*65536+v[9],age=v[6]*65536+v[7]);frames.append(item)
   if v[1]&6==6:assert (v[12],v[13])==(0,4096);valid.append(item)
   if v[1]&512:held.append(item)
   if not item['sequence']:before.append(item)
 else:assert a[2:6]==q[2:6] and len(a)==8
assert valid and held and before and held[-1]['status']==1 and not held[-1]['flags']&4
markers=[x for x in raw if x['kind'] in ('completed','collision_refused')];assert any(x['kind']=='collision_refused' and x['result']==27 for x in markers)
initial=(h/'fram-before.bin').read_bytes();final=(h/'live05-fram.bin').read_bytes();assert final==(p/'model-final.bin').read_bytes();assert final[:122880]==initial[:122880] and final[245760:]==initial[245760:]
ranges=[dict(zip(('at','bytes','commands','stage','status'),map(int,m))) for m in re.findall(r'\{at = (\d+), bytes = (\d+), commands = (\d+), stage = (\d+), status = (\d+)\}',log) if int(m[1])];assert len(ranges)==24 and sum(x['bytes'] for x in ranges)==b['write_bytes']==419408 and all(x['at']>=122880 and x['at']+x['bytes']<=245760 and not x['status'] for x in ranges)
assert u['stage']==10 and not u['error'] and u['starts']==1 and not b['fifo_error'] and not b['stop_error'] and w['max_work_us']<=1000 and w['max_service_us']<=2500 and b['max_gap_us']<=5000 and clock['max_residual_us']<=100 and not irq['events_dropped'] and not irq['error_callback'] and not irq['tx_fail_safe']
out=dict(result='PASS_BOUNDED_NONPRODUCTION',code_sha='806d967b912080872a44f51c1f75887448c62ea3',probe=u,bench=b,work=w,clock=clock,irq=irq,oracle=dict(N=4096,rms_mg=rms,peak_mg=peak,duration_ms=first['window_duration_ms'],exact=True),serial=dict(summary=summary,adu_count=len(adus),counts=counts,all_crc_length_ok=True,valid=len(valid),held=len(held),before_window=len(before),last_b3=frames[-1],markers=markers,latency_ms=dict(min=min(lat),max=max(lat),median=sorted(lat)[len(lat)//2])),fram_sha256=hashlib.sha256(final).hexdigest(),full_fram_exact_metadata_model=True,historical_and_tail_unchanged=True,write_ranges=ranges,physical_fram_calls=sum(x['commands'] for x in ranges),utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
(p/'physical-final-summary.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out,indent=2))
