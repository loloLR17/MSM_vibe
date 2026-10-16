from pathlib import Path
import re,json,hashlib,datetime
local=Path('/home/lolo/dev/msm/reports/TR2-r4m');p=local/'hardware'
def parse(log):
 s=log.read_text(errors='replace');out={}
 for block in s.split('R4M_LABEL ')[1:]:
  label=block.splitlines()[0].strip();body=block[len(label):].strip()
  out[label]=body
  if label in ['r4_result','work']:
   out[label]={k:(v=='true' if v in ('true','false') else int(v)) for k,v in re.findall(r'(\w+) = (\d+|true|false)(?=,|\s*})',body)}
  elif label.startswith('*r4m_'):
   out[label]=[{k:int(v) for k,v in re.findall(r'(\w+) = (\d+)',entry)} for entry in re.findall(r'\{([^{}]+)\}',body)]
  elif re.match(r'^\$\d+ = \d+\s*$',body):out[label]=int(body.split('=')[1])
 return out
r={}
for n in (4,6):
 f=p/f'r4m-observation{n}.log'
 if f.exists():r[f'physical_mode{n}']=parse(f)
if 'physical_mode6' in r:
 a=r['physical_mode6'];ranges=a.get('*r4m_ranges@r4m_range_count',[]);ops=a.get('*r4m_operations@r4m_op_count',[])
 a['write_audit']=dict(all_in_LIVE=all(x['at']>=122880 and x['at']+x['bytes']<=245760 for x in ranges),all_status_zero=all(x['status']==0 for x in ranges),bytes=sum(x['bytes'] for x in ranges),commands=sum(x['commands'] for x in ranges))
 for op in ops:op['duration_us']=op['end_us']-op['begin_us']
 r['overall_phase_C_pass']=a['r4_result'].get('stage')==10 and a['r4_result'].get('error')==0 and len(ops)==6 and all(x['status']==4 and x['result']==0 for x in ops)
for f in ['flash-before.bin','fram-before.bin','fram-after-physical.bin','fram-bridge-second-read.bin','fram-after-resume.bin','fram-after-reboot.bin','flash-restored.bin','fram-after-restoration.bin']:
 if (p/f).exists():r[f]=dict(bytes=(p/f).stat().st_size,sha256=hashlib.sha256((p/f).read_bytes()).hexdigest())
r['utc']=datetime.datetime.now(datetime.timezone.utc).isoformat();(local/'measurements.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps({k:v for k,v in r.items() if k not in ['physical_mode4','physical_mode6']},indent=2))
