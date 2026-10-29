#!/usr/bin/env python3
"""Offline raw DWT distributions and independent integer FIR replay, not a run."""
import argparse,json,math,re,struct,hashlib
from pathlib import Path
FIELDS='version stage error uid0 uid1 uid2 clock who run mode duration_us records span_us span_ticks cal_residual cal_brackets cal_anchors accepted outputs warmup rails clips source_hash output_hash first_us last_us residual clock_checks level flags stop_us stopped fifo_error stop_error trace_refused preclose_overflow irqs stack_deepest stack_floor primask basepri faultmask max_words raw_words pending_xyz ctrl1 cal_xyz_span cal_tick_span'.split()
def unpack_result(raw):
 if len(raw)!=4*len(FIELDS):raise ValueError('Result ABI length')
 r=dict(zip(FIELDS,struct.unpack('<'+'I'*len(FIELDS),raw)))
 if [r[x] for x in ('uid0','uid1','uid2')]!=[0x001b0046,0x38425018,0x20333157] or r['version']!=1 or r['clock']!=160000000:raise ValueError('Identity/ABI')
 return r
def stats(values):
 if not values:return None
 s=sorted(values);n=len(s)
 return dict(n=n,mean=sum(s)/n,p95=s[math.ceil(n*.95)-1],p99=s[math.ceil(n*.99)-1],maximum=s[-1])
def analyze(d):
 r=unpack_result((d/'result.bin').read_bytes());raw=(d/'records.bin').read_bytes()
 if len(raw)!=r['records']*16:raise ValueError('Record count')
 rows=list(struct.iter_unpack('<IIII',raw));out=dict(result=r,physical_pass=r['stage']==4 and not r['error'] and bool(r['stopped']))
 out['distributions']={key:stats([x[i] for x in rows]) for i,key in enumerate(['service_cycles','work_cycles','gap_cycles_quantized_us','xyz_per_poll'])}
 if rows and not r['duration_us']:raise ValueError('Missing duration')
 if rows:out['service_duty_percent']=100*sum(x[0] for x in rows)/(160*r['duration_us'])
 sources=list(struct.iter_unpack('<QQhhh??', (d/'sources.bin').read_bytes()))
 outputs=list(struct.iter_unpack('<QQQQhhhBBB7x',(d/'outputs.bin').read_bytes()))
 if sources:
  for i,s in enumerate(sources):
   if s[0]!=i or not s[5] or not s[6] or (i and s[1]<=sources[i-1][1]):raise ValueError('Source index/time')
  words=list(struct.iter_unpack('7B',(d/'raw.bin').read_bytes()))
  xyz=[struct.unpack('<hhh',bytes(b[1:])) for b in words if b[0]>>3==2]
  if [s[2:5] for s in sources]!=xyz[:len(sources)]:raise ValueError('Sensor/mapper XYZ disagreement')
  coeff_path=Path(__file__).resolve().parents[3]/'Modbus RTU/05_Firmware/src/domain/acquisition/fir_decimator10_coefficients.h'
  text=coeff_path.read_text().split('= {',1)[1].split('}',1)[0];c=list(map(int,re.findall(r'-?\d+',text)));assert len(c)==107
  checked=0
  for o in outputs:
   segment,index,oi,us,x,y,z,flags,rails,clips=o
   if index>=len(sources):continue
   expect=[]
   for axis in range(3):
    acc=sum(c[k]*sources[index-k][2+axis] for k in range(min(index+1,107)))
    q=(abs(acc)+16384)//32768;v=-q if acc<0 else q;expect.append(max(-32768,min(32767,v)))
   if (x,y,z)!=tuple(expect) or us!=sources[index][1] or index!=oi*10 or segment!=r['run'] or bool(flags&1)!=(index<106):raise ValueError('Independent FIR/causal replay disagreement')
   checked+=1
  out['independent_source_prefix']=len(sources);out['independent_fir_outputs']=checked
 out['sha256']={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(d.glob('*.bin'))}
 return out
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('capture',type=Path);a=p.parse_args();print(json.dumps(analyze(a.capture),indent=2))
