"""Bounded offline audit of archived extremes; never assign their physical cause."""
import argparse,re
from prequal import *

def audit(manifest):
 m=json.loads(Path(manifest).read_text());info=m['source'];assert sha(SOURCE)==EXPECTED
 thresholds=[2000,4000,6000,7000];n={v:np.zeros(3,np.int64) for v in thresholds};episodes={v:[] for v in thresholds};last={v:None for v in thresholds};best=[]
 for i,t,x in verified_blocks(SOURCE,info):
  valid=np.arange(i,i+len(t))>=11
  for v in thresholds:
   hit=(abs(x)>v)&valid[:,None];n[v]+=hit.sum(axis=0);ix=np.flatnonzero(hit.any(axis=1))
   for j in ix:
    # Group exceedances within 1 nominal MCU second, retaining true sample times.
    if last[v] is None or t[j]-last[v]>1:episodes[v].append(dict(start_mcu_s=float(t[j]),end_mcu_s=float(t[j]),count=0,max_abs_nominal_mg=0.))
    r=episodes[v][-1];r['end_mcu_s']=float(t[j]);r['count']+=1;r['max_abs_nominal_mg']=max(r['max_abs_nominal_mg'],float(max(abs(x[j]))));last[v]=t[j]
  j=int(np.argmax(abs(x).max(axis=1)));best.append(dict(index=i+j,time_mcu_s=float(t[j]),xyz_nominal_mg=x[j].tolist()))
 best=sorted(best,key=lambda r:max(abs(v) for v in r['xyz_nominal_mg']),reverse=True)[:5]
 p=ROOT/'Modbus RTU/05_Firmware/src/domain/acquisition/fir_decimator10_coefficients.h';q=np.array(list(map(int,re.findall(r'-?\d+',p.read_text().split('= {',1)[1].split('}',1)[0]))));l1=sum(abs(q))/32768
 peak=max(abs(v) for r in best for v in r['xyz_nominal_mg']);lower=(peak-.122)/l1
 assert sha(SOURCE)==EXPECTED
 return dict(scope='ARCHIVED_POST_FIR; nominal244ug/count; not native waveform or physical shock attribution',counts_per_axis={str(v):n[v] for v in thresholds},episodes={str(v):episodes[v] for v in thresholds},strongest_block_extremes=best,FIR_L1=l1,rounding_bound_mg=.122,lower_bound_max_native_mg_if_chain_correct=lower,conclusion='Under correct documented chain, output peak implies some contributing native code magnitude >4g nominal, despite possible FIR overshoot. This does not isolate sensor motion vs input corruption, nor reconstruct peaks.',source_unchanged=True)
if __name__=='__main__':
 a=argparse.ArgumentParser();a.add_argument('manifest');a.add_argument('output',type=Path);o=a.parse_args()
 if o.output.exists():raise ValueError('new audit output required')
 dump(o.output,audit(o.manifest))
