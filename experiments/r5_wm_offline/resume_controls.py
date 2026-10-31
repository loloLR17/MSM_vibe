"""Resume only independent controls; verify saved sufficient statistics, no re-filter."""
import argparse,csv,json,hashlib,subprocess,time,resource
from run_wm import *

def resume(saved,source,out):
 begin=time.perf_counter();saved,source,out=Path(saved),Path(source),Path(out);out.mkdir(exist_ok=False,parents=True)
 if sha(source)!=EXPECTED:raise ValueError('known historical source required')
 info=reader.verify(source)
 if info['status']!='COMPLETE' or info['sha256']!=PAYLOAD:raise ValueError('strict source invalid')
 defs=json.loads((saved/'definitions.json').read_text());sufficient=np.load(saved/'sufficient_statistics.npz');observed=sufficient['observed']
 if sum(observed)!=info['valid_xyz']:raise ValueError('aggregate coverage mismatch')
 with (saved/'metrics.csv').open() as f:raw=list(csv.DictReader(f))
 rows=[]
 for r in raw:
  for k in ['width_s','start_mcu_s','valid_n','observed_n']:r[k]=int(r[k])
  for k in ['rms_x','rms_y','rms_z','direction_max','vector_technical','equivalent_velocity_mm_s']:r[k]=float(r[k])
  r['TR2_WM_A_MAX']=float(r['TR2_WM_A_MAX']) if r['TR2_WM_A_MAX'] else None;rows.append(r)
 duration=info['observed_duration_us']/1e6
 for mid,d in defs.items():
  if definition(d['fs_est_Hz'],d['clock'],d['variant'])!=d:raise ValueError('cached method differs from current core')
  if hashlib.sha256(canonical(d).encode()).hexdigest()!=mid:raise ValueError('method identity')
  moment=Moments(len(observed));moment.n=sufficient[mid+'_n'];moment.ss=sufficient[mid+'_ss']
  for width in [60,120]:
   check=groups(moment,observed,duration,width,mid,d['variant'],d['clock'])
   for r in check:
    prior=next(a for a in rows if a['method_id']==mid and a['width_s']==width and a['start_mcu_s']==r['start_mcu_s'])
    if any(prior[k]!=r[k] for k in ['valid_n','observed_n','quality']):raise ValueError('cached quality mismatch')
    if not np.allclose([prior['rms_'+a] for a in 'xyz'],[r['rms_'+a] for a in 'xyz'],rtol=1e-12,equal_nan=True):raise ValueError('cached RMS mismatch')
 # Original successful filtering core identity must be unchanged from committed checkpoint.
 checkpoint='e7da1098';original=subprocess.check_output(['git','show',checkpoint+':experiments/r5_wm_offline/wm.py'],cwd=ROOT)
 if hashlib.sha256(original).hexdigest()!=sha(Path(__file__).with_name('wm.py')):raise ValueError('cached numerical core changed')
 controls(source,info,rows,defs,out,EVENT)
 if sha(source)!=EXPECTED or reader.verify(source)!=info:raise ValueError('source changed')
 artifacts={name:sha(saved/name) for name in ['definitions.json','metrics.csv','sufficient_statistics.npz','whole_period.json','event_influence.json']}
 dump(out/'MANIFEST.json',dict(mission_id='TR2-20261031-R5-WM-CONFORT-INDICATIF-OFFLINE',mission_revision='R1-20261031-PREFERENCE-BV',source=info,source_whole_sha256=EXPECTED,core_sha256=sha(Path(__file__).with_name('wm.py')),runner_sha256=sha(Path(__file__).with_name('run_wm.py')),resume_sha256=sha(__file__),cached_filter_code_commit=checkpoint,cached_artifacts=artifacts,clocks={d['clock']:d['fs_est_Hz'] for d in defs.values()},rows=len(rows),source_before_after_equal=True,filtering_repeated=False,performance_controls_only=dict(elapsed_s=time.perf_counter()-begin,peak_RSS_KiB=resource.getrusage(resource.RUSAGE_SELF).ru_maxrss),main_full_run_cost='failed Parseval gate after all filter aggregates saved; full-run peak/time not captured; observed mid-run RSS200208KiB only, not peak',classification='NON_CLASSEE',BV_comparison='NOT_YET_STUDIED'))
 print('Cached aggregates verified; independent controls and source invariants PASS',flush=True)
if __name__=='__main__':
 a=argparse.ArgumentParser();a.add_argument('saved');a.add_argument('output');o=a.parse_args();resume(o.saved,SOURCE,o.output)
