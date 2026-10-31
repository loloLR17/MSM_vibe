#!/usr/bin/env python3
"""Reusable archive-only TR2-Wm CLI; outputs nominal acceleration, not conformity."""
import argparse,csv,math,time,resource,subprocess,importlib.util
from scipy import fft,signal
from wm import *
import hashlib,json
spec=importlib.util.spec_from_file_location('prequal_verified_helpers',ROOT/'experiments/r5_prequal_offline/run.py');helpers=importlib.util.module_from_spec(spec);spec.loader.exec_module(helpers)
csvwrite=helpers.csvwrite;groups=helpers.groups;comparison=helpers.comparison
SOURCE=Path('/home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/pc-recovery-after-powercut/E6N0005.TR2')
EVENT=(17600.,17630.) # Diagnostic only, fixed BEFORE inspecting Wm results.

def parseval_error(Y,y,nfft):
 # Long-double accumulation avoids reduction-order error of float64 axis0 sums.
 weights=np.full(len(Y),2.,dtype=np.longdouble);weights[0]=1
 if nfft%2==0:weights[-1]=1
 spectral=np.sum(weights[:,None]*(np.asarray(Y.real,dtype=np.longdouble)**2+np.asarray(Y.imag,dtype=np.longdouble)**2),axis=0)/nfft
 temporal=np.sum(np.asarray(y,dtype=np.longdouble)**2,axis=0)
 return float(max(abs(temporal/spectral-1)))

def controls(source,info,rows,defs,out,event):
 rr=sorted([r for r in rows if r['name']=='MAIN' and r['clock']=='MCU' and r['width_s']==120 and r['quality']=='FULL'],key=lambda r:r['direction_max']);starts=sorted(set([rr[j]['start_mcu_s'] for j in [0,len(rr)//2,len(rr)-1]]+([int(event[0]//120)*120] if event is not None else [])))
 pieces={s:([],[]) for s in starts}
 for _,t,x in verified_blocks(source,info):
  for s,(tt,xx) in pieces.items():
   m=(t>=s-30)&(t<s+150)
   if m.any():tt.append(t[m]);xx.append(x[m]*UNIT)
 records=[]
 for s,(tt,xx) in pieces.items():
  t=np.concatenate(tt);x=np.concatenate(xx);nfft=fft.next_fast_len(len(x)+int(60*info['observed_output_hz_between_endpoints']));X=fft.rfft(x,n=nfft,axis=0)
  for mid,d in defs.items():
   fs=d['fs_est_Hz'];f=fft.rfftfreq(nfft,1/fs);H=oracle_digital(f,fs,d['variant']);Y=X*H[:,None];y=fft.irfft(Y,n=nfft,axis=0);weight=np.full(len(Y),2.);weight[0]=1
   if nfft%2==0:weight[-1]=1
   errP=parseval_error(Y,y,nfft)
   for width,start in [(120,s),(60,s),(60,s+60)]:
    take=(t>=start)&(t<start+width);r=np.sqrt(np.mean(y[:len(x)][take]**2,axis=0));target=next(a for a in rows if a['method_id']==mid and a['width_s']==width and a['start_mcu_s']==start);z=np.array([target['rms_'+a] for a in 'xyz']);err=max(abs(r/z-1));record=dict(method_id=mid,clock=d['clock'],variant=d['variant'],start_mcu_s=start,width_s=width,time_rms_mm_s2=z,FFT_rms_mm_s2=r,relative_error=err,limit=.001,Parseval_relative_error=errP,Parseval_limit=1e-12,passed=err<.001 and errP<1e-12);records.append(record)
    if not record['passed']:dump(out/'failed_controls.json',records);raise ValueError('independent real control failed')
 dump(out/'real_controls.json',records)

def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('output',type=Path);ap.add_argument('--source',type=Path,default=SOURCE);ap.add_argument('--expected-sha256',default=EXPECTED);ap.add_argument('--qpc-rate',type=float);a=ap.parse_args();out=a.output;out.mkdir(exist_ok=False,parents=True);begin=time.perf_counter()
 if sha(a.source)!=a.expected_sha256:raise ValueError('expected source SHA mismatch')
 info=reader.verify(a.source)
 if info['status']!='COMPLETE':raise ValueError('strict COMPLETE required')
 clocks={'MCU':info['observed_output_hz_between_endpoints']}
 if a.qpc_rate is not None:clocks['QPC_ESTIMATE']=a.qpc_rate
 elif a.expected_sha256==EXPECTED:
  witness=json.loads((ROOT/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261031-R5-ANALYSE-PC-MACHINE-6H/manifest.json').read_text());qpc=witness['QPC'];assert sha(qpc['path'])==qpc['sha256'];clocks['QPC_ESTIMATE']=sum(qpc['output_rate_est_Hz_bounds'])/2
 if a.expected_sha256==EXPECTED and info['sha256']!=PAYLOAD:raise ValueError('expected campaign payload mismatch')
 event=EVENT if a.expected_sha256==EXPECTED else None
 duration=info['observed_duration_us']/1e6;seconds=int(math.floor(duration))+1;observed=np.zeros(seconds,np.int64);defs={};streams={};moments={};without={};diagnostics=[]
 for clock,fs in clocks.items():
  for variant in ['MAIN','LOW08','LOW12']:
   d=definition(fs,clock,variant);mid=hashlib.sha256(canonical(d).encode()).hexdigest();defs[mid]=d;streams[mid]=Stream(fs,np.array(d['sos']));moments[mid]=Moments(seconds);without[mid]=Moments(seconds)
 print('Strict COMPLETE; six Wm streams; smooth band limits once; primary event retained',flush=True)
 for index,t,x in verified_blocks(a.source,info):
  observed+=np.bincount(np.floor(t).astype(int),minlength=seconds);valid=old.quality_mask(x,np.arange(index,index+len(x)));input_mm=x*UNIT
  for mid,s in streams.items():
   y=s.apply(input_mm,t,valid);moments[mid].add(t,y);without[mid].add(t,np.where(((t<event[0])|(t>=event[1]))[:,None],y,np.nan) if event is not None else y)
   if defs[mid]['variant']=='MAIN':
    finite=np.isfinite(y).all(axis=1);edges=np.flatnonzero(np.diff(np.r_[False,finite,False]));spans=list(zip(edges[::2],edges[1::2]));lo,hi=max(spans,key=lambda ab:ab[1]-ab[0]) if spans else (0,0);good=y[lo:hi]
    if len(good)>16*s.fs:
     f,P=signal.welch(good,fs=s.fs,nperseg=int(16*s.fs),axis=0,detrend=False);power=P.sum(axis=1);total=np.sum(power)
     diagnostics.append(dict(method_id=mid,start_mcu_s=float(t[0]),end_mcu_s=float(t[-1]),below1_fraction=float(sum(power[f<1])/total),above80_fraction=float(sum(power[f>80])/total),interpretation='16s Hann Welch approximate diagnostic including leakage; not a brickwall metric'))
 rows=[]
 for mid,m in moments.items():
  d=defs[mid]
  for width in [60,120]:
   for r in groups(m,observed,duration,width,mid,d['variant'],d['clock']):
    vals=[r['rms_'+a] for a in 'xyz'];r.update(TR2_WM_A_MAX=r['direction_max'] if d['variant']=='MAIN' else None,metric=d['metric'],unit='mm/s2',winning_axis='XYZ'[int(np.argmax(vals))] if np.isfinite(vals).all() else '',equivalent_velocity_mm_s=.028*r['direction_max'],classification='NON_CLASSEE');rows.append(r)
 dump(out/'definitions.json',defs);csvwrite(out/'metrics.csv',rows)
 np.savez_compressed(out/'sufficient_statistics.npz',observed=observed,**{mid+'_n':m.n for mid,m in moments.items()},**{mid+'_ss':m.ss for mid,m in moments.items()},**{mid+'_excluded_n':m.n for mid,m in without.items()},**{mid+'_excluded_ss':m.ss for mid,m in without.items()})
 summary,pairs,sensitivity=comparison(rows);dump(out/'summary.json',summary);csvwrite(out/'aligned_pairs.csv',pairs);dump(out/'duration_sensitivity.json',sensitivity);dump(out/'out_of_band_diagnostic.json',diagnostics)
 period=[];influence=[]
 for mid,m in moments.items():
  rm=pooled_rms(m.n,m.ss);w=without[mid];rw=pooled_rms(w.n,w.ss);d=defs[mid]
  period.append(dict(method_id=mid,variant=d['variant'],clock=d['clock'],valid_samples=int(sum(m.n)),rms_xyz_mm_s2=rm,max_direction_mm_s2=max(rm),winning_axis='XYZ'[int(np.argmax(rm))],equivalent_v_mm_s=.028*max(rm),scope='all valid samples including partial final window, not mean of RMS'))
  influence.append(dict(method_id=mid,variant=d['variant'],clock=d['clock'],excluded_mcu_s=event,main_valid_n=int(sum(m.n)),diagnostic_retained_n=int(sum(w.n)),main_rms_xyz=rm,diagnostic_excluding_event_rms_xyz=rw,relative_change_max_direction=max(rw)/max(rm)-1,main_energy_fraction_in_excluded_interval=(m.ss.sum(axis=0)-w.ss.sum(axis=0))/m.ss.sum(axis=0),policy='DIAGNOSTIC ONLY; no source edit/reset/inpainting; primary always retains event'))
  # Affected windows remain PARTIAL, not promoted to FULL after exclusion.
  for width in [60,120]:
   main={r['start_mcu_s']:r for r in rows if r['method_id']==mid and r['width_s']==width}
   for r in groups(w,observed,duration,width,mid,d['variant'],d['clock']):
    if event is not None and r['start_mcu_s']<event[1] and r['start_mcu_s']+width>event[0]:influence.append(dict(method_id=mid,width_s=width,start_mcu_s=r['start_mcu_s'],main=main[r['start_mcu_s']],diagnostic=r,policy='remaining sample RMS, reduced coverage; never a replacement FULL window'))
 dump(out/'whole_period.json',period);dump(out/'event_influence.json',influence)
 controls(a.source,info,rows,defs,out,event)
 if sha(a.source)!=a.expected_sha256 or reader.verify(a.source)!=info:raise ValueError('source altered during run')
 dump(out/'MANIFEST.json',dict(mission_id='TR2-20261031-R5-WM-CONFORT-INDICATIF-OFFLINE',mission_revision='R1-20261031-PREFERENCE-BV',source=info,source_whole_sha256=a.expected_sha256,core_sha256=sha(Path(__file__).with_name('wm.py')),runner_sha256=sha(__file__),git_code_head=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),clocks=clocks,rows=len(rows),source_before_after_equal=True,performance=dict(elapsed_s=time.perf_counter()-begin,peak_RSS_KiB=resource.getrusage(resource.RUSAGE_SELF).ru_maxrss),hardware_access=False,classification='NON_CLASSEE',BV_comparison='NOT_YET_STUDIED'))
 print('Full archive and independent controls PASS',flush=True)
if __name__=='__main__':main()
