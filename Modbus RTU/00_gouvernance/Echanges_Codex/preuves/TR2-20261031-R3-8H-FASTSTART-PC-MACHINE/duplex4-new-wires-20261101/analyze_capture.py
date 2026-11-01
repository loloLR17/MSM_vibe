from pathlib import Path
import json,struct,math,re,sys,hashlib
P=Path('/home/lolo/dev/msm/reports/TR2-r3-N8E-duplex4-20261101');sys.path.insert(0,str(P));import run_capture as c
ROOT=c.ROOT
coeff=list(map(int,re.findall(r'-?\d+',(ROOT/'Modbus RTU/05_Firmware/src/domain/acquisition/fir_decimator10_coefficients.h').read_text().split('= {')[1].split('}')[0])));assert len(coeff)==107
def stats(values):
 if not values:return None
 x=sorted(values);return dict(n=len(x),mean=sum(x)/len(x),p95=x[math.ceil(len(x)*.95)-1],p99=x[math.ceil(len(x)*.99)-1],maximum=x[-1])
def analyze(name):
 d=P/name;r=c.analyze(d);out=dict(result=r,pass_references=r['stage']==4 and not any(r[k] for k in ['error','fifo_error','stop_error','sd_error','trace_refused','preclose_overflow','queue_overflows','flags']) and r['stopped']==1)
 raw=(d/'distributions.bin').read_bytes();assert len(raw)==80096
 timing={}
 for i,key in enumerate(['service','work','gap','sd_step']):
  row=raw[i*20024:(i+1)*20024];total,n,m=struct.unpack_from('<QII',row);h=struct.unpack_from('<5001I',row,16);assert n==r['records'] and sum(h)==n
  def quantile(q):
   wanted=math.ceil(n*q);count=0
   for bin,f in enumerate(h):
    count+=f
    if count>=wanted:return bin
  timing[key]=dict(n=n,mean_cycles=total/n if n else None,max_cycles=m,mean_us=total/n/160 if n else None,max_us=m/160,p95_us_ceil=quantile(.95) if n else None,p99_us_ceil=quantile(.99) if n else None)
 out['timing']=timing
 if r['records']:assert timing['service']['max_us']<=2500 and timing['work']['max_us']<=1000 and timing['gap']['max_us']<=5000 and timing['sd_step']['max_us']<=1000
 sources=list(struct.iter_unpack('<QQhhh??',(d/'sources.bin').read_bytes()));outputs=list(struct.iter_unpack('<QQQQhhhBBB7x',(d/'outputs.bin').read_bytes()));words=list(struct.iter_unpack('7B',(d/'raw.bin').read_bytes()))
 if sources:
  assert [s[2:5] for s in sources]==[struct.unpack('<hhh',bytes(w[1:])) for w in words if w[0]>>3==2][:len(sources)]
  for i,v in enumerate(sources):assert v[0]==i and v[5] and v[6] and (not i or v[1]>sources[i-1][1])
 checked=0
 for o in outputs:
  segment,index,oi,us,x,y,z,flags,rails,clips=o
  if index>=len(sources):continue
  expected=[]
  for axis in range(3):
   acc=sum(coeff[k]*sources[index-k][2+axis] for k in range(min(index+1,107)));v=(abs(acc)+16384)//32768;v=-v if acc<0 else v;expected.append(max(-32768,min(32767,v)))
  assert tuple(expected)==(x,y,z) and segment==r['run'] and index==oi*10 and us==sources[index][1] and bool(flags&1)==(index<106);checked+=1
 out['numeric_prefix_verified_outputs']=checked;out['source_prefix_verified']=len(sources)
 sd=list(struct.iter_unpack('<5I' if r['version']>=2 else '<4I',(d/'sd-records.bin').read_bytes()));assert len(sd)==min(r['sd_completed'],2048)
 if r['sd_completed']<=2048:
  out['sd_write_us']=stats([x[0] for x in sd]);out['sd_busy_us']=stats([x[1] for x in sd])
 else:
  hist=(d/'sd-distributions.bin').read_bytes();assert len(hist)==8048
  for i,key in enumerate(['sd_write_us','sd_busy_us']):
   total,n,mx=struct.unpack_from('<QII',hist,i*4024);bins=struct.unpack_from('<1001I',hist,i*4024+16);assert n==r['sd_completed']==sum(bins)
   def qtile(q):
    target=math.ceil(n*q);acc=0
    for b,v in enumerate(bins):
     acc+=v
     if acc>=target:return b*(10000 if r['version']>=2 else 1000)
   out[key]=dict(n=n,mean=total/n,maximum=mx,p95_us_ceil_ms=qtile(.95),p99_us_ceil_ms=qtile(.99),raw_records_preserved=len(sd),quantiles=('All groups histogram ceil10ms' if r['version']>=2 else 'All writes histogram ceil1ms')+'; raw trace first256+last1792 only')
 if r['version']>=2:
  allbusy=(d/'busy-distributions.bin').read_bytes();assert len(allbusy)==8048
  for i,key in enumerate(['per_sector_busy','FD_stop_busy']):
   total,n,mx=struct.unpack_from('<QII',allbusy,i*4024);bins=struct.unpack_from('<1001I',allbusy,i*4024+16);assert n==sum(bins)
   assert n==(r['sd_written']//512 if i==0 else r['sd_completed'])
   def bqtile(q):
    target=math.ceil(n*q);acc=0
    for b,v in enumerate(bins):
     acc+=v
     if acc>=target:return b*1000
   out[key]=dict(n=n,mean_us=total/n if n else 0,max_us=mx,p95_us_ceil_ms=bqtile(.95),p99_us_ceil_ms=bqtile(.99))
  out['sd_record_unit']='One acknowledged group; per-sector and FD busy separate'
  if r['sd_completed']<=2048:assert sum(x[4] for x in sd)*512==r['sd_written']
 out['sha256_raw']={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in d.glob('*.bin')}
 (d/'analysis.json').write_text(json.dumps(out,indent=2)+'\n');return out
if __name__=='__main__':print(json.dumps(analyze(sys.argv[1]),indent=2))
