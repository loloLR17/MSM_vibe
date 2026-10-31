"""Offline comparison only. Reads preserved BIN/ELF metadata, snapshots and Git.
No build, test run, debugger, port, Windows, SD or signal computation.
"""
from pathlib import Path
import json,hashlib,subprocess,struct,datetime
R=Path('/home/lolo/dev/msm/projects/MSM_vibe-R3-8h-faststart');P=Path(__file__).parent
root={
 'E6':Path('/home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031'),
 'N8A':Path('/home/lolo/dev/msm/reports/TR2-r3-8h-faststart-20261031'),
 'N8B':Path('/home/lolo/dev/msm/reports/TR2-r3-8h-faststart-N8B-20261031'),
 'N8C':Path('/home/lolo/dev/msm/reports/TR2-r3-8h-faststart-N8C-20261031'),
 'N8D':Path('/home/lolo/dev/msm/reports/TR2-r3-8h-faststart-N8D-20261031')}
sha=lambda b:hashlib.sha256(b).hexdigest()
images={};bins={}
for name,p in root.items():
 a=json.loads((p/'artifact.json').read_text());b=(p/'arm/r3_coex.bin').read_bytes();assert sha(b)==a['images'][0]['sha256'];bins[name]=b
 flags=(p/'arm/CMakeFiles/r3_coex.dir/flags.make').read_text();defs=next(s for s in flags.splitlines() if s.startswith('C_DEFINES'));opt=next(s for s in flags.splitlines() if s.startswith('C_FLAGS'))
 images[name]=dict(source=a['code_sha'],BIN_bytes=len(b),BIN_sha256=sha(b),prefix=a['prefix'],guard_us=a['nominal_guard_us'],definitions=defs,compiler_flags=opt,source_path=str(p/'artifact.json'))
published_base='Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/'
published_paths={'N8A':published_base+'arm/r3_coex.bin','N8B':published_base+'rearm-after-reboot/N8B/arm/r3_coex.bin','N8C':published_base+'rearm-after-reboot/N8C/arm/r3_coex.bin','N8D':published_base+'duplex-differential/8MHz-high/arm/r3_coex.bin'}
for n,path in published_paths.items():
 published=subprocess.check_output(['git','show','97fa3d0a:'+path],cwd=R);assert published==bins[n];images[n]['published_BIN_identical']=True;images[n]['published_path']=path
comparisons=[]
for old,new in [('N8A','N8B'),('N8B','N8C')]:
 a=images[old];b=images[new];x=bins[old];y=bins[new];assert len(x)==len(y)
 allowed=set();fields=[]
 for key in ['source','prefix']:
  vx=a[key].encode();vy=b[key].encode();assert len(vx)==len(vy) and x.count(vx)==y.count(vy)==1
  ox=x.find(vx);oy=y.find(vy);assert ox==oy;allowed.update(range(ox,ox+len(vx)));fields.append(dict(field=key,offset=ox,length=len(vx),old=a[key],new=b[key]))
 diffs=[i for i,(v,w) in enumerate(zip(x,y)) if v!=w];assert set(diffs)<=allowed
 normx=x.replace(a['source'].encode(),b'0'*40).replace(a['prefix'].encode(),b'XXX');normy=y.replace(b['source'].encode(),b'0'*40).replace(b['prefix'].encode(),b'XXX');assert normx==normy
 fx=a['definitions'].replace(a['source'],'SOURCE').replace(a['prefix'],'PREFIX');fy=b['definitions'].replace(b['source'],'SOURCE').replace(b['prefix'],'PREFIX');assert fx==fy and a['compiler_flags']==b['compiler_flags']
 comparisons.append(dict(pair=[old,new],different_bytes=len(diffs),offsets=diffs,allowed_fields=fields,all_other_image_bytes_identical=True,normalized_image_sha256=sha(normx),normalized_build_definitions_identical=True,compiler_flags_identical=True))
steps=[]
for n,folder in [('N8A','n8a-card-calibrate'),('N8A','n8a-short-data'),('N8B','n8b-card-calibrate'),('N8B','n8b-failed-gate'),('N8C','n8c-card-calibrate')]:
 p=root[n]/folder;r=json.loads((p/'result.json').read_text());steps.append(dict(image=n,step=folder,result_file_sha256=sha((p/'result.json').read_bytes()),observation={k:r[k] for k in ['stage','error','clock','who','uid_0','uid_1','uid_2','cal_residual','cal_brackets','cal_anchors','duration_us','accepted','outputs','fifo_error','sd_error','rails','clips','primask','basepri','faultmask','sd_steps','sd_written','sd_completed','sd_pending_polls','sd_peak_queue']},SPI3_CFG1_CFG2=[hex(v) for v in struct.unpack('<II',(p/'spi3-config.bin').read_bytes())],SPI2_CFG1_CFG2=[hex(v) for v in struct.unpack('<II',(p/'spi2-config.bin').read_bytes())],CID=(p/'card.bin').read_bytes()[16:32].hex()))
assert len({tuple(s['SPI3_CFG1_CFG2']) for s in steps})==1 and len({tuple(s['SPI2_CFG1_CFG2']) for s in steps})==1
assert len({s['CID'] for s in steps})==1
scope=['experiments/r3_durable_fat32/src','experiments/r3_durable_fat32/CMakeLists.txt','experiments/iis3dwb_st_reference','experiments/r3_coex_fifo_fir_sd','experiments/r3_time_reference','experiments/spi2_microsd_isolated','Modbus RTU/05_Firmware'];gits=[]
for a,b in [('c3cd40ce','d5af0828'),('d5af0828','75ea886c')]:
 cmd=['git','diff','--exit-code',a,b,'--',*scope];z=subprocess.run(cmd,cwd=R,capture_output=True);assert z.returncode==0;gits.append(dict(command=cmd,return_code=0,stdout_sha256=sha(z.stdout),runtime_sources_unchanged=True))
events=[]
for n in ['N8A','N8B','N8C']:
 for f in sorted((root[n]/'hardware').glob('*invocation.json')):
  a=json.loads(f.read_text());events.append(dict(image=n,file=f.name,host_UTC_annotation=a['utc'],return_code=a['return_code']))
out=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),analysis_head=subprocess.check_output(['git','rev-parse','HEAD'],cwd=R,text=True).strip(),method='offline byte comparison, exact compiler defines, existing observations only',images=images,binary_comparisons=comparisons,observations=steps,git_source_comparisons=gits,host_events=sorted(events,key=lambda e:e['host_UTC_annotation']),limitations=['Host UTC annotations are not sample UTC or an independent physical clock measurement.','No first-post-move reset-cause RCC flags were saved; later resets prevent reliable retrospective attribution.','No successful N8A calibration was replayed after relocation with the exact saved N8A image.','Identical code does not exclude a pre-existing state/timing/data-dependent firmware defect.','Static 3.3V does not characterize SPI or supply transients.','Later duplex/slew/sensor trials are separate conditions; shared causal origin is not proved.'])
(P/'COMPARISON.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(dict(binary_comparisons=comparisons,SPI_snapshots_all_identical=True,offline_only=True),indent=2))
