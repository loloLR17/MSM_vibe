from pathlib import Path
import subprocess as s,json,hashlib,re,csv
root=Path.cwd();fw=root/'Modbus RTU/05_Firmware';out=Path('/home/lolo/dev/msm/reports/TR2-r3b')
initial=json.loads((out/'initial-state.json').read_text())
for q in initial['protected']:assert hashlib.sha256((root/q['path']).read_bytes()).hexdigest()==q['sha256'],q['path']
assert s.check_output(['git','stash','list','--format=%H']).decode().splitlines()==initial['stashes']
def old(name):return s.check_output(['git','show',initial['head']+':'+name]).decode()
def body(source,name):
 at=source.index(name+'(');start=source.index('{',at);end=start;depth=0
 for end in range(start,len(source)):
  depth+=(source[end]=='{')-(source[end]=='}')
  if not depth:break
 return source[start:end+1]
pairs=[('accumulation','Modbus RTU/05_Firmware/src/application/acquisition_service.c','acquisition_window_accumulate_valid_sample','Modbus RTU/05_Firmware/src/domain/acquisition/window_statistics.c','acquisition_window_accumulate_valid_sample'),('raw_conversion','Modbus RTU/05_Firmware/platform/stm32/stm32_iis3dwb_vibration_source.c','raw_to_mg','Modbus RTU/05_Firmware/include/tr2/platform/iis3dwb_units.h','tr2_iis3dwb_raw_to_mg')]
shared={}
for name,path,fn,newpath,newfn in pairs:
 a=body(old(path),fn);b=body((root/newpath).read_text(),newfn);assert a==b
 shared[name]={'body_sha256':hashlib.sha256(a.encode()).hexdigest(),'old':path,'new':newpath,'body_byte_identical':True}
for path in ['src/domain/supervision/indicator_calculator.c','src/modbus/projection.c']:
 name='Modbus RTU/05_Firmware/'+path;assert old(name)==(root/name).read_text();shared[path]={'file_unchanged':True}
(out/'shared-functions.json').write_text(json.dumps(shared,indent=2)+'\n')
# Field-by-field preparation status derived from unchanged R0 mapping.
status={0:'R1 identity prepared, V1 projector masks LIVE capability; not composed',1:'V1 layout; successor authorities/aggregator pending R4/composition',2:'existing time service; qualified sample clock correlation pending',3:'R3B in-memory projection implemented/tested; not routed RTU',4:'R1 projection/schema2/CRC implemented; not routed RTU',5:'R2 epoch 29-register interface implemented; historical RTU remains20',6:'V1 projector refuses unavailable4; successor adapter pending',7:'V1 layout; successor event/priority/persistence composition pending'}
with (root/'Modbus RTU/02_Validation/LIVE_R0/mapping_live.csv').open() as f:rows=list(csv.DictReader(f))
for q in rows:q['implementation_status']=status[int(q['block'])]
(out/'projection-readiness.json').write_text(json.dumps(rows,indent=2)+'\n')
# Inventory refresh, retaining R3A classifications except newly compiled modules.
oldinv=json.loads((root/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261015-R3A/r3a-module-inventory.json').read_text())
print('old inventory schema',type(oldinv).__name__,list(oldinv)[:6] if isinstance(oldinv,dict) else len(oldinv))
print('protected',len(initial['protected']),'shared bodies exact','mapping',len(rows))
