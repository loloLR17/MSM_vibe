from pathlib import Path
import re,json
p=Path('/home/lolo/dev/msm/reports/TR2-r4/hardware');runs={}
for f in p.glob('*result.log'):
 text=f.read_bytes().decode('utf-8','replace');fields=re.search(r'\$1 = \{(.*?)\}',text)
 if not fields:continue
 d={k:int(v) for k,v in re.findall(r'(\w+) = (\d+)',fields[1])}
 if 'stage' not in d:continue
 d['terminal_success']=d['stage']==10 and d['error']==0
 d['qualification_elapsed_ms']=(d['tick_end']-d['tick_start'])&0xffffffff
 w=re.search(r'\$2 = \{(.*?)\}',text)
 if w:d['cooperative_work']={k:int(v) for k,v in re.findall(r'(\w+) = (\d+)',w[1])}
 runs[f.stem]=d
(p/'measurements.json').write_text(json.dumps(runs,indent=2)+'\n');print(json.dumps(runs,indent=2))
