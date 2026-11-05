"""Independent V1.9 release evidence. Reads actual board/CAM/source extraction; no hardware."""
from pathlib import Path
import json,math,hashlib,subprocess,zipfile,csv,re,ast
P=Path(__file__).resolve().parents[1];ROOT=Path(subprocess.check_output(['git','rev-parse','--show-toplevel'],cwd=P,text=True).strip());S=P.parent;Q=P/'preuves';BASE='ad4b698f42a7b174e92227a88ce68d8c9c8935b1'
read=lambda f:json.loads(f.read_text());sha=lambda b:hashlib.sha256(b).hexdigest();save=lambda f,x:f.write_text(json.dumps(x,ensure_ascii=False,indent=2)+'\n')
B=read(P/'docs/checks/pcb-readback.json');ST=read(Q/'ST-coordinates.json');T=read(Q/'comparaison-synthese.json')['transform_translation_mm'];mp={'J1':'CN7','J2':'CN8','J3':'CN9','J4':'CN10','J5':'CN12'};pads={(p['ref'],p['number']):p for p in ST['headers']};deltas=[]
for p in B['pads']:
 if p['ref']not in mp or not p['number']:continue
 n=str(64+int(p['number'])) if p['ref']=='J5'else p['number'];src=pads[mp[p['ref']],n]['xy_ST_mm'];want=[src[0]+T[0],T[1]-src[1]];delta=math.dist([p['x'],p['y']],want);assert delta<.000002,(p,want,delta);deltas.append(delta)
assert len(deltas)==106
assert read(Q/'interfaces-ST.json')['native_CN12_69_net']=='PG4'
for x in B['pads']:
 if x['ref']=='J5':assert x['net']=='RTU_DE'if x['number']=='5'else x['net'].startswith('unconnected-')
old=read(S/'cad/routes.json');new=read(P/'cad/routes.json');changed=[i for i,(a,b)in enumerate(zip(old['routes'],new['routes']))if a!=b];assert changed==[8,11,13,14,15,16,17]
for a,b in zip(old['routes'],new['routes']):assert (a['net'],a['from'],a['to'])==(b['net'],b['from'],b['to'])
assert len(new['routes'])==32
# Historical dossier and all its tests/docs/CAM remain byte-identical to initial origin/main.
frozen=[]
for row in subprocess.check_output(['git','ls-tree','-rz',BASE,'--',str(S.relative_to(ROOT))],cwd=ROOT).split(b'\0'):
 if not row:continue
 meta,name=row.split(b'\t');name=name.decode();b=(ROOT/name).read_bytes();oldb=subprocess.check_output(['git','cat-file','blob',meta.split()[2].decode()],cwd=ROOT)
 if name==str((S/'README.md').relative_to(ROOT)):
  assert b.endswith(oldb),'historical README content lost';continue
 assert b==oldb,name;frozen.append({'path':name,'sha256':sha(b),'bytes':len(b)})
save(Q/'historique-conserve.json',{'base':BASE,'count':len(frozen),'files':frozen})
for ext in ['.kicad_sch','.kicad_pro']:assert (P/('cad/TR2_Shield_Proto_V1'+ext)).read_bytes()==(S/('cad/TR2_Shield_Proto_V1'+ext)).read_bytes()
D=read(P/'design.json');oldD=read(S/'design.json');assert D['board_outline_mm']==oldD['board_outline_mm'];assert D['retainer_mount_holes']==oldD['retainer_mount_holes']
for a,b in zip(oldD['components'],D['components']):
 if a['ref']not in ['J1','J5']:assert a==b
for key in ['violations','unconnected_items','schematic_parity']:assert not read(P/'docs/checks/DRC.json').get(key,[])
assert not any(x.get('violations')for x in read(P/'docs/checks/ERC.json')['sheets']);assert read(P/'docs/checks/export-verification.json')['status']=='PASS'
# Compare drill coordinates by one-to-one matching at the writer's actual1um metric resolution.
max_err=0
for kind,npth in [('NPTH',True),('PTH',False)]:
 want=[(x['x'],x['y'],x['drill'])for x in B['pads']if x['npth']==npth]
 if not npth:want +=[(x['x'],x['y'],.3)for x in B['tracks']if x['via']]
 tools={};got=[];tool=None
 for line in (P/f'exports/BLOCKED_FABRICATION/TR2_Shield_Proto_V1-{kind}.drl').read_text().splitlines():
  m=re.fullmatch(r'T(\d+)C([\d.]+)',line)
  if m:tools[m[1]]=float(m[2])
  elif re.fullmatch(r'T\d+',line):tool=line[1:]
  else:
   m=re.fullmatch(r'X(-?[\d.]+)Y(-?[\d.]+)',line)
   if m:got.append((float(m[1]),-float(m[2]),tools[tool]))
 assert len(got)==len(want)
 for x in got:
  idx=min(range(len(want)),key=lambda i:math.dist(x,want[i]));v=want.pop(idx);assert abs(x[2]-v[2])<.000001;err=math.dist(x[:2],v[:2]);assert err<=.000708;max_err=max(max_err,err)
# Normalize only new text for Git; retain all token sequences and old artifacts unchanged.
for f in P.rglob('*'):
 if f.is_file()and f.suffix.lower()in ['.json','.log','.py','.txt','.csv','.md','.gbl','.gtl','.gbs','.gts','.gbo','.gto','.gm1','.drl','.gbrjob','.kicad_pcb','.kicad_sch','.kicad_pro','.kicad_mod']:
  b=f.read_bytes();s=b.decode('utf8');newb=('\n'.join(x.rstrip()for x in s.splitlines())+'\n').encode();assert b.split()==newb.split();f.write_bytes(newb)
cam=P/'exports/BLOCKED_FABRICATION';files=sorted(f for f in cam.iterdir()if f.suffix in ['.gbl','.gtl','.gbs','.gts','.gbo','.gto','.gm1','.drl','.gbrjob']);assert len(files)==10
zname=P/'exports/TR2_SHIELD_V19_NON_LIBERE.zip'
with zipfile.ZipFile(zname,'w',compression=zipfile.ZIP_DEFLATED)as z:
 for f in files:
  info=zipfile.ZipInfo(f.name,date_time=(2026,11,5,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED;z.writestr(info,f.read_bytes())
with zipfile.ZipFile(zname)as z:
 assert z.testzip()is None
 for f in files:assert z.read(f.name)==f.read_bytes()
save(P/'exports/CAM-manifeste.json',{'release':'NON_LIBERE_NO_GO_PHYSICAL','source_head':BASE,'board_sha256':sha((P/'cad/TR2_Shield_Proto_V1.kicad_pcb').read_bytes()),'files':[{'name':f.name,'sha256':sha(f.read_bytes()),'bytes':f.stat().st_size}for f in files],'ZIP':zname.name,'ZIP_sha256':sha(zname.read_bytes())})
for f in P.rglob('*.json'):json.loads(f.read_text())
for f in P.rglob('*.py'):ast.parse(f.read_text())
result={'status':'PASS_NOMINAL_CAD_ONLY','ST_native_vs_ODB_pins':ST['header_pads_compared_to_independent_ODB'],'source_delta_mm':ST['max_ODB_delta_mm'],'shield_header_pins_compared':len(deltas),'max_native_C05_to_corrected_shield_delta_mm':max(deltas),'changed_routes':changed,'unchanged_routes':32-len(changed),'all_modules_schematic_project_rules_outline_retainer_holes':'UNCHANGED','old_shield_files_preserved':len(frozen),'ERC':0,'DRC':0,'unconnected':0,'schematic_parity':0,'CAM_files':10,'Excellon_max_center_quantization_error_mm':max_err,'physical_gate_XY':'NOT_VERIFIED_CORRECTED_PRINT','physical_gate_Z':'NOT_VERIFIED_G_AND_MATING','fabrication':'NO_GO','zip_sha256':sha(zname.read_bytes())};save(Q/'validation-finale.json',result);print(json.dumps(result,indent=2))
