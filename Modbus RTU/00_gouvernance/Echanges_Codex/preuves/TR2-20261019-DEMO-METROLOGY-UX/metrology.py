from pathlib import Path
import struct,math,json,hashlib,sqlite3,re
p=Path(__file__).parent
results=[]
for n in [1,2]:
 f=Path('/home/lolo/dev/msm/reports/TR2-windows-trust-live-resume/hardware')/f'cycle{n}-audit-xyz.bin'
 raw=list(struct.iter_unpack('<hhh',f.read_bytes()))
 mg=[[(-1 if v<0 else 1)*((abs(v)*61+500)//1000) for v in x] for x in raw]
 means=[sum(x[i] for x in mg)/len(mg) for i in range(3)]
 sq=[sum(x[i]**2 for x in mg) for i in range(3)]
 rms=[math.isqrt(sum(sq)//len(raw))]+[math.isqrt(s//len(raw)) for s in sq]
 peaks=[math.isqrt(max(sum(v*v for v in x) for x in mg))]+[max(abs(x[i]) for x in mg) for i in range(3)]
 expected=([962,955,102,61],[1000,991,135,103]) if n==1 else ([962,955,101,62],[1002,992,145,106])
 assert (rms,peaks)==expected
 ac=[math.sqrt(s/len(raw)-m*m) for s,m in zip(sq,means)]
 results.append(dict(source=str(f),sha256=hashlib.sha256(f.read_bytes()).hexdigest(),historical=True,xyz=len(raw),full_scale_code=0,sensitivity_ug_lsb=61,rms_mg=rms,peak_mg=peaks,raw_minmax=[[min(x[i] for x in raw),max(x[i] for x in raw)] for i in range(3)],raw_saturation_count=sum(v in (-32768,32767) for x in raw for v in x),mean_axis_mg=means,dc_norm_mg=math.sqrt(sum(m*m for m in means)),offline_centered_rms_axis_mg=ac,offline_centered_rms_global_mg=math.sqrt(sum(x*x for x in ac)),centered_diagnostic_only_not_B3=True))
db=sqlite3.connect('file:/mnt/c/Users/lolo/AppData/Local/MSM/TR2-DEMO-METROLOGY-UX/baseline.db?mode=ro',uri=True)
valid=[]
for rid,payload in db.execute('SELECT row_id,payload_json FROM live_observation'):
 o=json.loads(payload);s=o.get('Snapshot')
 if s and s['ValuesValid'] and s['Fresh'] and not s['Held']:valid.append((rid,o))
from collections import defaultdict
by=defaultdict(list)
for rid,o in valid:by[o['ReceivedAt'][:10]].append(o['Snapshot']['Value']['RmsGlobalMg'])
out=dict(raw_oracles=results,sqlite_integrity=db.execute('pragma integrity_check').fetchone()[0],sqlite_valid_fresh_observations=len(valid),by_day={k:dict(count=len(v),rms_min_mg=min(v),rms_max_mg=max(v),span_mg=max(v)-min(v)) for k,v in by.items()},conclusion='No 2 mg cap in conversion/B3 or graph domain. Existing Y scale 0..max. 2mg user observation not exactly reproducible without its original time/graph; small variation is not FS. DC not removed; startup calibration is time only. No normative change.')
(p/'metrology-offline.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out,indent=2))
