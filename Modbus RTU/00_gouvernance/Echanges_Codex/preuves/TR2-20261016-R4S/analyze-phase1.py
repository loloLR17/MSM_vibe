from pathlib import Path
import json,re,struct,math,hashlib,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-r4s');h=p/'hardware';log=(h/'phase1-trigger12-result.log').read_text(errors='replace')
def numbers(line):return {k:int(v) for k,v in re.findall(r'(\w+) = ([0-9]+)(?=[,}])',line)}
lines=log.splitlines();s=numbers([l for l in lines if re.match(r'\$\d+ = \{stage = .*write_attempts =',l)][-1]);r=numbers([l for l in lines if re.match(r'\$\d+ = \{stage = .*words =',l)][-1]);b=numbers([l for l in lines if re.match(r'\$\d+ = \{stage = .*line =',l)][-1]);w=numbers([l for l in lines if re.match(r'\$\d+ = \{context = .*work_limit_us',l)][-1]);paired=numbers([l for l in lines if re.match(r'\$\d+ = \{total = ',l)][-1]);clock=numbers([l for l in lines if re.match(r'\$\d+ = \{fifo = ',l)][-1]);first=re.search(r'FIRST_RMS=([0-9,]+) PEAK=([0-9,]+) N=(\d+) DURATION=(\d+)',log);n=int(first[3]);oracle=None
if n==4096:
 xyz=list(struct.iter_unpack('<hhh',(h/'phase1-xyz.bin').read_bytes()));assert len(xyz)==4096
 def mg(v):return (1 if v>=0 else -1)*((abs(v)*61+500)//1000)
 xyz=[tuple(mg(v) for v in row) for row in xyz];squares=[sum(row[j]**2 for row in xyz) for j in range(3)];rms=[math.isqrt(sum(squares)//n)]+[math.isqrt(v//n) for v in squares];peak=[math.isqrt(max(sum(v*v for v in row) for row in xyz))]+[max(abs(row[j]) for row in xyz) for j in range(3)]
 assert rms==list(map(int,first[1].split(','))) and peak==list(map(int,first[2].split(',')))
 oracle=dict(result='PASS',N=n,rms_mg=rms,peak_mg=peak,physical_values=True,diagnostic_only=True,R2_admission=False)
assert (h/'fram-before.bin').read_bytes()==(h/'fram-after-phase1.bin').read_bytes()
passed=s['stage']==10 and not s['error'] and not r['error'] and not s['write_attempts'] and not b['write_bytes'] and b['xyz']>4096 and w['max_work_us']<=1000 and w['max_service_us']<=2500 and b['max_gap_us']<=5000 and oracle is not None and r['valid_responses']==0
out=dict(phase1='PASS' if passed else 'FAILED',diagnostic=s,shared=r,bench=b,work=w,paired_max=paired,clock=clock,oracle=oracle,fram_full_exact=True,fram_sha256=hashlib.sha256((h/'fram-after-phase1.bin').read_bytes()).hexdigest(),utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
(p/'phase1-summary.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out,indent=2))
