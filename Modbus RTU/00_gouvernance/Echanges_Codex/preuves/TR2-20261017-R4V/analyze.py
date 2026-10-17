from pathlib import Path
import re,json,sys,hashlib,struct,math,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-r4v');mode=sys.argv[1];h=p/'hardware';text=(h/(mode+'-phase.log')).read_text(errors='replace');lines=text.splitlines()
def nums(s):return {k:int(v) for k,v in re.findall(r'(\w+) = ([0-9]+)(?=[,}])',s)}
def pick(s):return nums([x for x in lines if re.match(s,x)][-1])
u=pick(r'\$\d+ = \{stage = .*mode =');v=pick(r'\$\d+ = \{event_count =');b=pick(r'\$\d+ = \{stage = .*line =');w=pick(r'\$\d+ = \{context = .*work_limit_us');clock=pick(r'\$\d+ = \{fifo =');irq=pick(r'\$\d+ = \{uart_irq =')
events=[nums(x) for x in re.findall(r'\{us = [^}]+\}',text) if 'kind = 0,' not in x];spans=[nums(x) for x in re.findall(r'\{begin_us = [^}]+\}',text) if 'begin_us = 0,' not in x]
assert len(events)==v['event_count'] and len(spans)==v['span_count']
commands=[]
for tx in ([5,6] if mode=='baseline' else [7,9]):
 capture=[e for e in events if e['kind']==2 and e['tx']==tx][0];code=capture['code'];cs=[s for s in spans if s['tx']==tx and s['journal_after']>s['journal_before']];assert len(cs)==4
 start=next(e for e in events if e['kind']==3 and e['code']==code);end=next(e for e in events if e['kind']==4 and e['code']==code);observed=next(e for e in events if e['kind']==7 and e['code']==code)
 owner=sum(s['end_us']-s['begin_us'] for s in cs);srv=sum(s['service_us'] for s in cs);io=sum(s['io_us'] for s in cs)
 c=dict(tx=tx,code=code,capture_us=capture['us'],admitted_us=cs[0]['end_us'],started_durable_us=cs[1]['end_us'],effect_begin_us=start['us'],effect_end_us=end['us'],observed_us=observed['us'],receipt_us=cs[2]['end_us'],terminal_us=cs[3]['end_us'],capture_to_effect_end_us=end['us']-capture['us'],capture_to_terminal_us=cs[3]['end_us']-capture['us'],callback_us=end['us']-start['us'],owner_us=owner,service_us=srv,io_us=io,cpu_and_instrumentation_residual_us=owner-srv-io,service_fraction=srv/owner,spans=cs)
 if code==14:c.update(held_published_us=v['held_publish_us'],stop_to_held_us=v['held_publish_us']-end['us'],capture_to_held_us=v['held_publish_us']-capture['us'])
 commands.append(c)
raw=[json.loads(x) for x in (h/(mode+'-master.jsonl')).read_text().splitlines()];adus=[x for x in raw if x['kind']=='adu'];summary=[x for x in raw if x['kind']=='summary'][-1];assert summary['passed']
def crc(a):
 v=65535
 for b in a:
  v^=b
  for _ in range(8):v=(v>>1)^40961 if v&1 else v>>1
 return v
held=None;ack=None
for row in adus:
 q=bytes.fromhex(row['request']);a=bytes.fromhex(row['response']);assert crc(q)==crc(a)==0 and q[:2]==a[:2]
 if q[1]==3:
  assert len(a)==a[2]+5 and a[2]==2*int.from_bytes(q[4:6],'big')
  if q[2:4]==b'\x0b\xb8':
   r=struct.unpack('>48H',a[3:-2]);assert not r[1]&64
   if ack and not held and r[0]==1 and r[1]&512 and not r[1]&4:held=row
 else:
  assert len(a)==8 and a[2:6]==q[2:6]
  if q[2:4]==b'\x13\x88' and int.from_bytes(q[9:11],'big')==commands[1]['tx'] and int.from_bytes(q[7:9],'big')==14:ack=row
assert ack and held
before=(h/'fram-before.bin').read_bytes();after=(h/(mode+'-fram.bin')).read_bytes();assert after==(p/('model-'+mode+'.bin')).read_bytes();assert before[:122880]==after[:122880] and before[245760:]==after[245760:]
first=pick(r'\$\d+ = \{configuration =');xyz=list(struct.iter_unpack('<hhh',(h/(mode+'-xyz.bin')).read_bytes()));conv=lambda n:(1 if n>=0 else -1)*((abs(n)*61+500)//1000);xyz=[tuple(conv(n) for n in row) for row in xyz];sq=[sum(row[i]**2 for row in xyz) for i in range(3)];rms=[math.isqrt(sum(sq)//4096)]+[math.isqrt(n//4096) for n in sq];peak=[math.isqrt(max(sum(n*n for n in row) for row in xyz))]+[max(abs(row[i]) for row in xyz) for i in range(3)]
assert rms==[first['rms_global_mg'],first['rms_x_mg'],first['rms_y_mg'],first['rms_z_mg']] and peak==[first['peak_global_mg'],first['peak_x_mg'],first['peak_y_mg'],first['peak_z_mg']]
assert u['stage']==10 and not u['error'] and not v['dropped'] and not b['fifo_error'] and not b['stop_error'] and w['max_work_us']<=1000 and w['max_service_us']<=2500 and b['max_gap_us']<=5000 and not irq['events_dropped'] and not irq['error_callback']
out=dict(result='PASS_BOUNDED',mode=mode,probe=u,stats=v,bench=b,work=w,clock=clock,irq=irq,events=events,commands=commands,master=summary,markers=[x for x in raw if x['kind'] in ['completed','collision_refused','budget_refused']],host_stop_ack_to_held_s=(datetime.datetime.fromisoformat(held['utc'])-datetime.datetime.fromisoformat(ack['utc'])).total_seconds(),fram_sha256=hashlib.sha256(after).hexdigest(),full_fram_exact_model=True,historical_tail_unchanged=True,oracle=dict(N=4096,rms_mg=rms,peak_mg=peak,exact=True))
(p/(mode+'-summary.json')).write_text(json.dumps(out,indent=2)+'\n')
for c in commands:print('tx',c['tx'],'effect',c['capture_to_effect_end_us']/1e6,'terminal',c['capture_to_terminal_us']/1e6,'service share',c['service_fraction'],'io',c['io_us']/1e6)
print('hostSTOP_ACK_HELD',out['host_stop_ack_to_held_s'],'FRAM',out['fram_sha256'],'budgets',w['max_work_us'],w['max_service_us'],b['max_gap_us'])
