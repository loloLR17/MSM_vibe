from pathlib import Path
import re,json,hashlib,datetime
local=Path('/home/lolo/dev/msm/reports/TR2-r4p');p=local/'hardware'
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
for n in (4,8):
 f=p/f'r4p-observation{n}.log'
 if f.exists():r[f'physical_mode{n}']=parse(f)
if 'physical_mode8' in r:
 a=r['physical_mode8'];ranges=a.get('*r4m_ranges@r4m_range_count',[]);ops=a.get('*r4m_operations@r4m_op_count',[])
 a['write_audit']=dict(all_in_LIVE=all(x['at']>=122880 and x['at']+x['bytes']<=245760 for x in ranges),all_status_zero=all(x['status']==0 for x in ranges),bytes=sum(x['bytes'] for x in ranges),commands=sum(x['commands'] for x in ranges))
 for op in ops:op['duration_us']=op['end_us']-op['begin_us']
 r['overall_phase_C_pass']=a['r4_result'].get('stage')==10 and a['r4_result'].get('error')==0 and len(ops)==6 and all(x['status']==4 and x['result']==0 for x in ops)
if 'physical_mode8' in r:
 a=r['physical_mode8'];v=a['r4_result'];w=a['work'];audit=a['write_audit'];ops=a.get('*r4m_operations@r4m_op_count',[])
 a['physical_limits']=dict(work_pass=w.get('max_work_us',1001)<=1000,service_pass=w.get('max_service_us',2501)<=2500,gap_pass=v.get('max_gap_us',5001)<=5000,no_deadline=not w.get('deadline_missed',True))
 a['write_audit_complete']=audit['bytes']==v.get('write_bytes') and audit['commands']==a.get('r4m_cs_closed') and audit['all_in_LIVE'] and audit['all_status_zero']
 a['run60_observed_us']=ops[3]['begin_us']-ops[2]['end_us'] if len(ops)>3 else None
 a['protected_scan_complete']=a.get('r4m_protected_checks',0)>=2 and a.get('r4m_protected_bytes')==a.get('r4m_protected_checks',0)*139264 and v.get('protected_equal')==1
 r['overall_phase_C_pass']=r['overall_phase_C_pass'] and a['protected_scan_complete'] and all(a['physical_limits'].values()) and a['write_audit_complete'] and v.get('fifo_error')==0 and v.get('stop_error')==0 and a.get('r4m_xyz_all',0)>0
for f in ['flash-before.bin' ,'fram-before.bin','fram-after-physical.bin','fram-after-readonly.bin','fram-after-resume.bin','fram-bridge-second-read.bin','fram-after-resume.bin','fram-after-reboot.bin','flash-restored.bin','fram-after-restoration.bin']:
 if (p/f).exists():r[f]=dict(bytes=(p/f).stat().st_size,sha256=hashlib.sha256((p/f).read_bytes()).hexdigest())
r['utc']=datetime.datetime.now(datetime.timezone.utc).isoformat();(local/'measurements.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps({k:v for k,v in r.items() if k not in ['physical_mode4','physical_mode4']},indent=2))
