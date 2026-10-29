#!/usr/bin/env python3
"""Offline only: raw RAM ABI, measured distributions and independent media oracle."""
import argparse,hashlib,json,math,re,struct,zlib
from pathlib import Path
MAIN=Path(__file__).resolve().parents[1]/'src/main.c'
def fields():
 s=MAIN.read_text();body=s[s.index(' uint32_t version,stage'):s.index('} R2Result;')];out=[]
 for group in re.findall(r'uint32_t\s+([^;]+);',body):
  for part in group.split(','):
   m=re.fullmatch(r'(\w+)(?:\[(\d+)\])?',part.strip());assert m
   out.extend([m[1]+'_'+str(i) for i in range(int(m[2]))] if m[2] else [m[1]])
 return out
def result(raw):
 f=fields()
 if len(raw)!=4*len(f):raise ValueError('Result ABI mismatch')
 r=dict(zip(f,struct.unpack('<'+'I'*len(f),raw)))
 if [r['uid_'+str(i)] for i in range(3)]!=[0x001b0046,0x38425018,0x20333157] or r['version']!=1 or r['hclk']!=160000000:raise ValueError('Identity/ABI')
 return r
def stats(v):
 if not v:return None
 v=sorted(v);return dict(n=len(v),mean=sum(v)/len(v),p95=v[math.ceil(len(v)*.95)-1],p99=v[math.ceil(len(v)*.99)-1],maximum=v[-1])
def expected(size,seed):
 out=bytearray()
 for word in range((size+3)//4):
  x=word^seed;x^=x>>16;x=x*0x7feb352d&0xffffffff;x^=x>>15;x=x*0x846ca68b&0xffffffff;x^=x>>16;out.extend(x.to_bytes(4,'little'))
 return bytes(out[:size])
def integrity(raw,size,seed):
 n=(size+511)//512*512;want=expected(size,seed)+b'\xff'*(n-size)
 if len(raw)!=n or raw!=want:raise ValueError('Media length/bytes/padding mismatch')
 return dict(payload_bytes=size,physical_bytes=n,all_bytes_exact=True,actual_sha256=hashlib.sha256(raw[:size]).hexdigest(),expected_sha256=hashlib.sha256(want[:size]).hexdigest(),padded_sha256=hashlib.sha256(raw).hexdigest(),actual_crc32=zlib.crc32(raw[:size]))
def analyze(d,media=None):
 r=result((d/'result.bin').read_bytes());raw=(d/'latencies.bin').read_bytes()
 if len(raw)!=r['records']*20:raise ValueError('Latency ABI/count')
 rows=list(struct.iter_unpack('<5I',raw));out=dict(result=r,physical_pass=r['stage']==4 and not any(r[k] for k in ['error','timeouts','crc_errors','preclose_error','overflows']) and r['cs_high']==1)
 out['latencies_by_kind']={str(kind):{key:stats([x[i] for x in rows if x[0]==kind]) for key,i in [('write_us',1),('busy_us',2),('backlog_bytes',3),('read_us',4)] if kind or key!='read_us'} for kind in sorted({x[0] for x in rows})}
 out['all_write_us']=stats([x[1] for x in rows]);out['phase_busy_us']=stats([x[2] for x in rows])
 if r['elapsed_us']:out['payload_rate_including_final_flush_Bps']=r['written']*1e6/r['elapsed_us'];out['sum_write_time_us']=sum(x[1] for x in rows)
 if media is not None:out['independent_integrity']=integrity(media.read_bytes(),r['written'],r['seed'])
 out['raw_sha256']={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(d.glob('*.bin'))}
 return out
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('capture',type=Path);p.add_argument('--media',type=Path);a=p.parse_args();print(json.dumps(analyze(a.capture,a.media),indent=2))
