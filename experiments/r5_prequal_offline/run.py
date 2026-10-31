#!/usr/bin/env python3
import argparse,csv,gzip,math,time,resource,subprocess
import numpy as np
from scipy import fft,signal,stats
from prequal import *

def csvwrite(p,rows):
 with (gzip.open(p,'wt',newline='') if str(p).endswith('.gz') else p.open('w',newline='')) as f:
  w=csv.DictWriter(f,fieldnames=list(rows[0]),lineterminator='\n');w.writeheader();w.writerows(rows)

def groups(moment,observed,duration,width,mid,name,clock):
 g=moment.groups(width);obs=np.add.reduceat(observed,g['start_s']);rows=[]
 for i,start in enumerate(g['start_s']):
  full=start+width<=duration+1e-3 and obs[i]>0 and g['n'][i]==obs[i]
  rows.append(dict(method_id=mid,name=name,clock=clock,width_s=width,start_mcu_s=int(start),observed_n=int(obs[i]),valid_n=int(g['n'][i]),quality='FULL' if full else 'PARTIAL_OR_INVALID',rms_x=g['rms'][i,0],rms_y=g['rms'][i,1],rms_z=g['rms'][i,2],direction_max=max(g['rms'][i]),vector_technical=g['global_rms'][i]))
 return rows

def comparison(rows):
 summaries=[];pairs=[]
 for mid in sorted({r['method_id'] for r in rows}):
  subset=[r for r in rows if r['method_id']==mid];a={r['start_mcu_s']:r for r in subset if r['width_s']==60 and r['quality']=='FULL'}
  b={r['start_mcu_s']:r for r in subset if r['width_s']==120 and r['quality']=='FULL'}
  for width in [60,120]:
   rr=[r for r in subset if r['width_s']==width and r['quality']=='FULL'];v=np.array([r['direction_max'] for r in rr]);n=np.array([r['valid_n'] for r in rr]);xyz=np.array([[r['rms_'+a] for a in 'xyz'] for r in rr]);ss=xyz**2*n[:,None]
   summaries.append(dict(method_id=mid,name=subset[0]['name'],clock=subset[0]['clock'],width_s=width,full_windows=len(rr),P50=np.median(v),P95=np.quantile(v,.95),maximum=max(v),period_rms_xyz=pooled_rms(n,ss),period_direction=max(pooled_rms(n,ss)),scope='only FULL windows; 60/120 FULL populations differ at edges'))
  for start,r in b.items():
   if start not in a or start+60 not in a:continue
   x,y=a[start],a[start+60];n=x['valid_n']+y['valid_n'];pooled=np.sqrt((np.array([x['rms_'+k] for k in 'xyz'])**2*x['valid_n']+np.array([y['rms_'+k] for k in 'xyz'])**2*y['valid_n'])/n)
   target=np.array([r['rms_'+k] for k in 'xyz']);err=max(abs(pooled/target-1))
   if err>1e-11:raise ValueError('energy pooling violated')
   pairs.append(dict(method_id=mid,name=r['name'],clock=r['clock'],start_mcu_s=start,pooled_direction=max(pooled),direct_120_direction=r['direction_max'],relative_energy_reconstruction_error=err,first_60_direction=x['direction_max'],second_60_direction=y['direction_max'],max60_to_120=max(x['direction_max'],y['direction_max'])/r['direction_max']))
 sensitivity=[]
 for mid in sorted({r['method_id'] for r in pairs}):
  pp=[r for r in pairs if r['method_id']==mid];direct=np.array([r['direct_120_direction'] for r in pp]);max60=np.array([max(r['first_60_direction'],r['second_60_direction']) for r in pp]);ratio=max60/direct
  sensitivity.append(dict(method_id=mid,name=pp[0]['name'],clock=pp[0]['clock'],pairs=len(pp),maximum_pooling_relative_error=max(r['relative_energy_reconstruction_error'] for r in pp),max60_to120_P50=np.median(ratio),max60_to120_P95=np.quantile(ratio,.95),max60_to120_max=max(ratio),rank_spearman_max60_vs120=stats.spearmanr(max60,direct).statistic,rank_scope='same 120s periods; max60 vs pooled120',pair_population_P50_120=np.median(direct),pair_population_P95_120=np.quantile(direct,.95)))
 return summaries,pairs,sensitivity

def controls(source,info,rows,definitions,out):
 # Same content: 3 low/median/high aligned 120s periods, flat MCU only for selection.
 rr=sorted([r for r in rows if r['name']=='FLAT_1_80' and r['clock']=='MCU' and r['width_s']==120 and r['quality']=='FULL'],key=lambda r:r['direction_max'])
 starts=[rr[i]['start_mcu_s'] for i in [0,len(rr)//2,len(rr)-1]];pieces={s:([],[]) for s in starts}
 for _,t,x in verified_blocks(source,info):
  for s,(tt,xx) in pieces.items():
   m=(t>=s-30)&(t<s+150)
   if m.any():tt.append(t[m]);xx.append(x[m])
 checks=[]
 for s,(tt,xx) in pieces.items():
  t=np.concatenate(tt);x=np.concatenate(xx);nfft=fft.next_fast_len(len(x)+int(60*info['observed_output_hz_between_endpoints']))
  X=fft.rfft(x,n=nfft,axis=0)
  for mid,d in definitions.items():
   fs=d['fs_est_Hz'];f=fft.rfftfreq(nfft,1/fs);H=transfer(d['sos'],f,fs);Y=X*H[:,None]
   y=fft.irfft(Y,n=nfft,axis=0)[:len(x)]
   # Independent PSD/Parseval exact identity over entire padded linear output.
   factor=np.full(len(Y),2.);factor[0]=1
   if nfft%2==0:factor[-1]=1
   spectral_energy=np.sum(factor[:,None]*abs(Y)**2,axis=0)/nfft
   full=fft.irfft(Y,n=nfft,axis=0);parseval_err=max(abs(np.sum(full*full,axis=0)/spectral_energy-1))
   for width,start in [(120,s),(60,s),(60,s+60)]:
    take=(t>=start)&(t<start+width);rms=np.sqrt(np.mean(y[take]**2,axis=0))
    target=next(r for r in rows if r['method_id']==mid and r['width_s']==width and r['start_mcu_s']==start)
    z=np.array([target['rms_'+a] for a in 'xyz']);err=max(abs(rms/z-1))
    rec=dict(method_id=mid,name=d['name'],clock=d['clock'],start_mcu_s=start,width_s=width,causal_rms=z,independent_fft_rms=rms,relative_error=err,limit_relative=.001,parseval_relative_error=parseval_err,parseval_limit=1e-12,passed=err<.001 and parseval_err<1e-12)
    checks.append(rec)
    if not rec['passed']:dump(out/'real_controls_failure.json',checks);raise ValueError('real FFT/PSD control failed')
 dump(out/'real_controls.json',checks);return checks

def main():
 ap=argparse.ArgumentParser();ap.add_argument('output',type=Path);a=ap.parse_args();out=a.output;out.mkdir(parents=True,exist_ok=False);begin=time.perf_counter()
 if sha(SOURCE)!=EXPECTED:raise ValueError('immutable source mismatch')
 info=reader.verify(SOURCE);assert info['status']=='COMPLETE' and info['sha256']==PAYLOAD
 witness=json.loads((ROOT/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261031-R5-ANALYSE-PC-MACHINE-6H/manifest.json').read_text());qpc=witness['QPC'];assert sha(qpc['path'])==qpc['sha256']
 clocks=dict(MCU=info['observed_output_hz_between_endpoints'],QPC_ESTIMATE=sum(qpc['output_rate_est_Hz_bounds'])/2)
 duration=info['observed_duration_us']/1e6;seconds=int(math.floor(duration))+1;observed=np.zeros(seconds,np.int64)
 definitions={};streams={};moments={}
 for clock,fs in clocks.items():
  for name in BANDS:
   d=definition(fs,name,clock);mid=hashlib.sha256(canonical(d).encode()).hexdigest();definitions[mid]=d;streams[mid]=Stream(fs,np.array(d['sos']));moments[mid]=Moments(seconds)
 hist=np.zeros((65536,3),np.int64);hvalid=np.zeros_like(hist);raw_min=np.full((seconds,3),np.inf);raw_max=np.full((seconds,3),-np.inf);raw_sum=np.zeros(3);raw_n=0
 print('Strict source accepted; 16 new streams with inherited quality/settling, no hardware',flush=True)
 for index,t,x in verified_blocks(SOURCE,info):
  ids=np.floor(t).astype(int);observed+=np.bincount(ids,minlength=seconds);counts=np.rint(x/.244).astype(np.int32);valid=old.quality_mask(x,np.arange(index,index+len(x)))
  for axis in range(3):
   hist[:,axis]+=np.bincount(counts[:,axis]+32768,minlength=65536);hvalid[:,axis]+=np.bincount(counts[np.arange(len(x))+index>=11,axis]+32768,minlength=65536)
  v=np.arange(len(x))+index>=11;ii=ids[v];xx=x[v];starts=np.r_[0,np.flatnonzero(np.diff(ii))+1];bins=ii[starts]
  raw_min[bins]=np.minimum(raw_min[bins],np.minimum.reduceat(xx,starts,axis=0));raw_max[bins]=np.maximum(raw_max[bins],np.maximum.reduceat(xx,starts,axis=0));raw_sum+=xx.sum(axis=0);raw_n+=len(xx)
  for mid,stream in streams.items():moments[mid].add(t,stream.apply(x,t,valid))
 rows=[]
 for mid,m in moments.items():
  for width in [60,120]:rows+=groups(m,observed,duration,width,mid,definitions[mid]['name'],definitions[mid]['clock'])
 # Preserve successful full-stream work BEFORE supplemental controls.
 dump(out/'definitions.json',definitions);csvwrite(out/'metrics.csv',rows)
 np.savez_compressed(out/'sufficient_statistics.npz',observed=observed,hist_all=hist,hist_postwarmup=hvalid,raw_min=raw_min,raw_max=raw_max,**{mid+'_n':m.n for mid,m in moments.items()},**{mid+'_ss':m.ss for mid,m in moments.items()})
 raw=dict(unit='nominal mg assuming logical8g',all_count=int(hist[:,0].sum()),postwarmup_count=raw_n,mean_postwarmup=raw_sum/raw_n,min_postwarmup=np.min(raw_min,axis=0),max_postwarmup=np.max(raw_max,axis=0),quantiles_all=hist_quantiles(hist,[0,.001,.01,.5,.99,.999,1]),quantiles_postwarmup=hist_quantiles(hvalid,[0,.001,.01,.5,.99,.999,1]),warmup_excluded_from_physical_extrema=11,native_raw_archived=False,temperature_archived=False,source_rail_flag=bool(info['quality_or']&2),output_clip_flag=bool(info['quality_or']&4),margin_to_fs_g={str(g):g-np.maximum(abs(np.min(raw_min,axis=0)),abs(np.max(raw_max,axis=0)))/1000 for g in [2,4,8]})
 dump(out/'raw_distribution.json',raw)
 rawrows=[dict(start_mcu_s=i,**{f'min_{a}_mg':raw_min[i,j] for j,a in enumerate('xyz')},**{f'max_{a}_mg':raw_max[i,j] for j,a in enumerate('xyz')}) for i in range(seconds)];csvwrite(out/'raw_1s.csv.gz',rawrows)
 summaries,pairs,sensitivity=comparison(rows);dump(out/'summary.json',summaries);csvwrite(out/'aligned_pairs.csv',pairs);dump(out/'duration_sensitivity.json',sensitivity)
 # Summed independent Butterworth band powers overlap: quantify, never claim partition.
 bandparts=[]
 for clock in clocks:
  for width in [60,120]:
   for total in [r for r in rows if r['name']=='FLAT_1_80' and r['clock']==clock and r['width_s']==width and r['quality']=='FULL']:
    parts=[next(r for r in rows if r['name']==name and r['clock']==clock and r['width_s']==width and r['start_mcu_s']==total['start_mcu_s']) for name in list(BANDS)[:4]]
    e=np.array([r['vector_technical']**2 for r in parts]);bandparts.append(dict(clock=clock,width_s=width,start_mcu_s=total['start_mcu_s'],sum_band_energy_to_total=e.sum()/total['vector_technical']**2,**{name:float(e[j]/e.sum()) for j,name in enumerate(list(BANDS)[:4])}))
 csvwrite(out/'band_shares.csv',bandparts)
 checks=controls(SOURCE,info,rows,definitions,out)
 if sha(SOURCE)!=EXPECTED or reader.verify(SOURCE)!=info:raise ValueError('source changed during analysis')
 manifest=dict(mission_id='TR2-20261031-R5-PREQUAL-ISO-FS2G-OFFLINE',source=info,source_whole_sha256=EXPECTED,core_sha256=sha(Path(__file__).with_name('prequal.py')),runner_sha256=sha(__file__),git_code_head=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),clocks=clocks,performance=dict(elapsed_s=time.perf_counter()-begin,peak_RSS_KiB=resource.getrusage(resource.RUSAGE_SELF).ru_maxrss),rows=len(rows),controls=len(checks),source_before_after_equal=True,existing_analyses_rerun=False,normative_comfort=False)
 dump(out/'MANIFEST.json',manifest);print(canonical(manifest['performance']),flush=True)
if __name__=='__main__':main()
