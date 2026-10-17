from pathlib import Path
import json,re,struct,math,hashlib,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-r4t');h=p/'hardware';log=(h/'phase5-composed-result.log').read_text(errors='replace');lines=log.splitlines()
def nums(l):return {k:int(v) for k,v in re.findall(r'(\w+) = ([0-9]+)(?=[,}])',l)}
def pick(pattern):return nums([l for l in lines if re.match(pattern,l)][-1])
r=pick(r'\$\d+ = \{stage = .*old_key_rejected =');shared=pick(r'\$\d+ = \{stage = .*words =');b=pick(r'\$\d+ = \{stage = .*line =');w=pick(r'\$\d+ = \{context = .*work_limit_us');paired=pick(r'\$\d+ = \{total =');clock=pick(r'\$\d+ = \{fifo =')
f=re.search(r'FIRST_RMS=([0-9,]+) PEAK=([0-9,]+) N=(\d+) DURATION=(\d+)',log);oracle=None
if f and int(f[3])==4096:
 xyz=list(struct.iter_unpack('<hhh',(h/'phase5-xyz.bin').read_bytes()));assert len(xyz)==4096
 def mg(v):return (1 if v>=0 else -1)*((abs(v)*61+500)//1000)
 xyz=[tuple(mg(v) for v in row) for row in xyz];n=4096;sq=[sum(row[j]**2 for row in xyz) for j in range(3)]
 rms=[math.isqrt(sum(sq)//n)]+[math.isqrt(v//n) for v in sq];peak=[math.isqrt(max(sum(v*v for v in row) for row in xyz))]+[max(abs(row[j]) for row in xyz) for j in range(3)]
 assert rms==list(map(int,f[1].split(','))) and peak==list(map(int,f[2].split(',')))
 oracle=dict(result='PASS',N=n,rms_mg=rms,peak_mg=peak,duration_ms=int(f[4]),physical_raw_values=True)
def crc(data):
 v=65535
 for b in data:
  v^=b
  for _ in range(8):v=(v>>1)^40961 if v&1 else v>>1
 return v
adus={}
for name in ('valid','held'):
 raw=(h/('phase5-'+name+'-adu.bin')).read_bytes()
 if not any(raw):adus[name]=dict(observed=False);continue
 assert len(raw)==101 and crc(raw)==0 and raw[:3]==bytes([1,3,96])
 regs=list(struct.unpack('>48H',raw[3:99]));assert not regs[1]&64
 if name=='valid':assert regs[1]&6==6 and ((regs[12]<<16)|regs[13])==4096
 else:assert regs[1]&512
 adus[name]=dict(observed=True,CRC='PASS',registers=regs,status=regs[0],flags=regs[1],age_ms=(regs[6]<<16)|regs[7],sequence=(regs[8]<<16)|regs[9])
a=(h/'fram-after-composed.bin').read_bytes();initial=(h/'fram-before.bin').read_bytes();protected=a[:122880]==initial[:122880] and a[245760:]==initial[245760:];assert protected
passed=r['stage']==10 and not r['error'] and not shared['error'] and b['xyz']>4096 and not b['fifo_error'] and not b['stop_error'] and w['max_work_us']<=1000 and w['max_service_us']<=2500 and b['max_gap_us']<=5000 and shared['valid_responses']>0 and shared['held_responses']>0 and oracle is not None and adus['valid']['observed'] and adus['held']['observed']
ranges=[dict(zip(('at','bytes','commands','stage','status'),map(int,m))) for m in re.findall(r'\{at = (\d+), bytes = (\d+), commands = (\d+), stage = (\d+), status = (\d+)\}',log) if int(m[1])]
assert len(ranges)==36 and sum(x['bytes'] for x in ranges)==b['write_bytes']==629112
assert all(x['at']>=122880 and x['at']+x['bytes']<=245760 and not x['status'] for x in ranges)
assert a==(p/'model-final.bin').read_bytes()
metrics=next(l for l in lines if re.match(r'\$\d+ = \{active = \{generation =',l))
assert 'authority = true' in metrics and 'run_id = 6' in metrics
trace=pick(r'\$\d+ = \{stage = .*write_attempts =')
out=dict(result='PASS' if passed else 'FAILED',recovery=r,shared=shared,bench=b,work=w,paired_max=paired,clock=clock,oracle=oracle,adus=adus,fram_sha256=hashlib.sha256(a).hexdigest(),historical_tail_exact=protected,utc=datetime.datetime.now(datetime.timezone.utc).isoformat());out.update(write_ranges=ranges,physical_write_calls=sum(x['commands'] for x in ranges),physical_fram_exact_model=True,windows=shared['b3_sequence'],clock_checks=trace['clock_checks'],context=dict(run=6,config_id=3,config_generation=3,authority=True))
(p/'physical-summary.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out,indent=2))
