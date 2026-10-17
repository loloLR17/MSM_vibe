from pathlib import Path
import json,struct,hashlib,math,re
p=Path('/home/lolo/dev/msm/reports/TR2-r4u');h=p/'hardware'
def crc(data):
 v=65535
 for b in data:
  v^=b
  for _ in range(8):v=(v>>1)^40961 if v&1 else v>>1
 return v
log=[json.loads(x) for x in (h/'ro04-master.jsonl').read_text(errors='replace').splitlines()]
adus=[x for x in log if x['kind']=='adu'];counts={};lat=[]
for x in adus:
 q=bytes.fromhex(x['request']);r=bytes.fromhex(x['response']);assert crc(q)==crc(r)==0 and r[:2]==b'\x01\x03' and r[2]+5==len(r)
 addr=int.from_bytes(q[2:4],'big');counts[addr]=counts.get(addr,0)+1;lat.append(x['elapsed_ms'])
 regs=struct.unpack('>'+str(r[2]//2)+'H',r[3:-2]);assert int.from_bytes(q[4:6],'big')==len(regs)
 if addr==3000:assert not regs[1]&6 and not regs[1]&64 and regs[4]==regs[5]==0 and regs[12:14] in ((0,0),(0,4096))
 if addr==6000:assert regs[0]==2 and regs[9]==4 and all(v==0 for i,v in enumerate(regs) if i not in [0,9])
b=(h/'ro04-failed-words.bin').read_bytes()[:161];valid=11
shifted=bytes((b[i]>>1)|((b[i-1]&1)<<7) for i in range(valid*7,len(b)))
tags=[shifted[i] for i in range(0,len(shifted),7)];assert all(t.bit_count()%2==0 and t>>3 in (2,4) for t in tags)
lastts=int.from_bytes(b[6*7+1:6*7+5],'little');newts=int.from_bytes(shifted[4*7+1:4*7+5],'little');assert newts-lastts==24
out=dict(result='FAILED_SAFE',requests=73,responses=len(adus),counts=counts,crc_lengths_all_ok=True,latency_ms=dict(min=min(lat),max=max(lat),median=sorted(lat)[len(lat)//2]),failure='FIFO BENCH_TAG9 parity reason1 after131050XYZ; realignment by one bit from word11 reconstructs remaining tags and timestamp+24ticks',failed_word=11,failed_tag='23',realigned_tags=[hex(t) for t in tags],raw_sha256=hashlib.sha256(b).hexdigest(),no_fram_mutation=(h/'ro04-fram.bin').read_bytes()==(h/'fram-before.bin').read_bytes(),cause='Undetermined: bitstream shift observed, no physical capture of SCK/MISO or proof of IRQ causality',budgets_us=dict(work=338,service=1452,gap=1386),stopped=True)
(p/'readonly-failure-analysis.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out,indent=2))
