"""Whole-period energy and edge/clock sensitivity from saved sufficient statistics."""
import argparse,csv
from prequal import *

def compute(results,output):
 results,output=Path(results),Path(output);defs=json.loads((results/'definitions.json').read_text());d=np.load(results/'sufficient_statistics.npz');period=[]
 for mid,method in defs.items():
  n=d[mid+'_n'];ss=d[mid+'_ss'];r=pooled_rms(n,ss)
  period.append(dict(method_id=mid,name=method['name'],clock=method['clock'],valid_samples=int(sum(n)),rms_xyz_mg=r,direction_max_mg=max(r),vector_technical_mg=np.linalg.norm(r),scope='all valid samples incl partial final window; 11warmup +10s settling excluded, no fill'))
 with (results/'metrics.csv').open() as stream:rows=list(csv.DictReader(stream))
 lookup={(r['name'],r['clock'],r['width_s'],r['start_mcu_s']):r for r in rows};sensitivity=[]
 for name in ['FLAT_1_80','TEST_LP10']:
  rr=[r for r in rows if r['name']==name and r['clock']=='MCU' and r['width_s']=='60' and r['quality']=='FULL'];ratios=[float(lookup[name,'QPC_ESTIMATE','60',r['start_mcu_s']]['direction_max'])/float(r['direction_max'])-1 for r in rr]
  sensitivity.append(dict(name=name,clock_relative_min=min(ratios),clock_relative_median=np.median(ratios),clock_relative_max=max(ratios)))
 dump(output,dict(whole_period=period,clock=sensitivity,source='saved sufficient_statistics.npz, no new full-stream filter'))
if __name__=='__main__':
 a=argparse.ArgumentParser();a.add_argument('results');a.add_argument('output');o=a.parse_args();compute(o.results,o.output)
