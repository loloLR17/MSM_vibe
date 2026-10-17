from pathlib import Path
import json,re,struct,hashlib,math
p=Path('/home/lolo/dev/msm/reports/TR2-r5afix');h=p/'hardware';text=(h/'fix-phase.log').read_text(errors='replace');lines=text.splitlines()
def nums(s):return {k:int(v) for k,v in re.findall(r'(\w+) = ([0-9]+)(?=[,}])',s)}
def pick(s):return nums([x for x in lines if re.match(s,x)][-1])
u=pick(r'\$\d+ = \{stage = .*mode =');bench=pick(r'\$\d+ = \{stage = .*line =');work=pick(r'\$\d+ = \{context = .*work_limit_us');irq=pick(r'\$\d+ = \{uart_irq =');first=pick(r'\$\d+ = \{configuration =')
assert u['stage']==10 and not u['error'] and u['starts']==1
assert not bench['fifo_error'] and not bench['stop_error'] and work['max_work_us']<=1000 and work['max_service_us']<=2500 and bench['max_gap_us']<=5000
assert not irq['events_dropped'] and not irq['error_callback'] and not irq['tx_fail_safe']
def crc(a):
 v=65535
 for b in a:
  v^=b
  for _ in range(8):v=(v>>1)^40961 if v&1 else v>>1
 return v
rows=[json.loads(x) for x in (h/'fix-master.jsonl').read_text().splitlines()];adus=[x for x in rows if x['kind']=='adu'];b3=[];commands=[]
for x in adus:
 q=bytes.fromhex(x['request']);a=bytes.fromhex(x['response']);assert crc(q)==crc(a)==0 and q[:2]==a[:2]==bytes([1,q[1]])
 if q[1]==3:
  count=int.from_bytes(q[4:6],'big');assert len(a)==2*count+5 and a[2]==2*count
  if int.from_bytes(q[2:4],'big')==3000:
   v=struct.unpack('>48H',a[3:-2]);assert not any(v[40:]) and v[3]<=4 and not v[1]&0xE100 and v[2]<=511
   expected=0 if v[1]&6!=6 else 4 if v[2]&15 else 3 if v[2]&240 else 2 if not v[1]&64 else 1
   assert v[3]==expected and not v[1]&64 and v[4]==v[5]==0
   assert not (v[1]&512 and v[1]&4)
   b3.append(dict(utc=x['utc'],words=list(v),alarms3002=v[2],severity3003=v[3],valid=bool(v[1]&2),fresh=bool(v[1]&4),held=bool(v[1]&512)))
 else:
  assert q[1]==16 and a[2:6]==q[2:6] and len(a)==8
  if q[2:4]==bytes([0x13,0x88]):commands.append(dict(code=int.from_bytes(q[7:9],'big'),tx=int.from_bytes(q[9:11],'big')))
assert commands==[dict(code=13,tx=10),dict(code=14,tx=11)]
valid=[x for x in b3 if x['valid'] and x['fresh']];held=[x for x in b3 if x['held']];assert valid and held and any(x['alarms3002'] for x in valid)
summary=next(x for x in rows if x['kind']=='summary');assert summary['passed'] and summary['requests']==len(adus)
fram=(h/'fix-fram.bin').read_bytes();before=(h/'fram-before.bin').read_bytes();assert fram==(p/'model-final.bin').read_bytes() and fram[:122880]==before[:122880] and fram[245760:]==before[245760:]
raw=list(struct.iter_unpack('<hhh',(h/'fix-xyz.bin').read_bytes()));assert len(raw)==4096
conv=lambda n:(1 if n>=0 else -1)*((abs(n)*61+500)//1000);xyz=[tuple(conv(x) for x in row) for row in raw];sq=[sum(row[i]**2 for row in xyz) for i in range(3)];rms=[math.isqrt(sum(sq)//4096)]+[math.isqrt(x//4096) for x in sq];peak=[math.isqrt(max(sum(x*x for x in row) for row in xyz))]+[max(abs(row[i]) for row in xyz) for i in range(3)]
assert rms==[first['rms_global_mg'],first['rms_x_mg'],first['rms_y_mg'],first['rms_z_mg']] and peak==[first['peak_global_mg'],first['peak_x_mg'],first['peak_y_mg'],first['peak_z_mg']]
v=dict(result='PASS_BOUNDED_NONPRODUCTION',probe=u,bench=bench,work=work,irq=irq,master=summary,commands=commands,aduCount=len(adus),B3Count=len(b3),validFreshCount=len(valid),heldCount=len(held),firstValid=valid[0],lastHeld=held[-1],fram_sha256=hashlib.sha256(fram).hexdigest(),full_fram_exact_model=True,historical_tail_unchanged=True,first4096_oracle=dict(rms_mg=rms,peak_mg=peak,exact=True),metadata=dict(journal=70,root=83,epoch=5,run=10,config_id=3,config_generation=3));(p/'physical-summary.json').write_text(json.dumps(v,indent=2)+'\n');print('Physical PASS',len(adus),'ADU',len(valid),'V/F',len(held),'HELD','budgets',work['max_work_us'],work['max_service_us'],bench['max_gap_us'],'FRAM',v['fram_sha256'])
