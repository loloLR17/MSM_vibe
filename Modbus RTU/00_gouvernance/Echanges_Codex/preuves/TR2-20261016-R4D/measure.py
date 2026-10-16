from pathlib import Path
import re,json,hashlib
p=Path('/home/lolo/dev/msm/reports/TR2-r4d');out={}
def rows(s):return [{k:int(v) for k,v in re.findall(r'(\w+) = (-?\d+)',t)} for t in re.findall(r'\{([^{}]+)\}',s)]
for name in ['before','after']:
 f=p/'hardware'/f'r4d-{name}-results.log'
 if not f.exists():continue
 text=f.read_text(errors='replace');blocks={int(n):s for n,s in re.findall(r'\$(\d+) = (.*)',text)}
 stats=rows(blocks[1])[0];exercises=rows(blocks[2]);traces=rows(blocks[3]);assert stats['stage']==100 and not stats['error']
 for x in exercises:
  for n in ['start','observe','stop','combined']:x[n+'_us']=x[n+'_cycles']/160
  assert x['stopped']==1 and not x['observe_status'] and not x['stop_status']
 d=dict(stats=stats,exercises=exercises,transfers=traces,instrumentation_extra_us_per_callback=(stats['calibration_trace']-stats['calibration_direct'])/128/160)
 if name=='before':
  assert len(exercises)==4 and exercises[3]['deadline']==1
  d['trace_groups']={str(stage):dict(callbacks=len([x for x in traces if x['stage']==stage]),total_measured_io_us=sum(x['cycles'] for x in traces if x['stage']==stage)/160,max_callback_us=max(x['cycles'] for x in traces if x['stage']==stage)/160) for stage in [1,2,3]}
 else:
  assert len(exercises)==2 and exercises[0]['kind']==4 and not exercises[0]['deadline'] and not exercises[0]['work_error'] and exercises[0]['max_work_us']<=1000
  assert exercises[1]['kind']==5 and exercises[1]['start_status']!=0 and sum(x['result']!=0 for x in traces)==1
 out[name]=d
for n in ['flash-before.bin','flash-restored.bin','fram-before.bin','fram-after-before.bin','fram-after-corrected.bin','fram-after-restoration.bin']:
 f=p/'hardware'/n
 if f.exists():out[n]=dict(bytes=f.stat().st_size,sha256=hashlib.sha256(f.read_bytes()).hexdigest())
(p/'measurements.json').write_text(json.dumps(out,indent=2)+'\n')
for name,d in out.items():
 if name in ['before','after']:print(name,d['stats'],d['exercises'],d['instrumentation_extra_us_per_callback'])
