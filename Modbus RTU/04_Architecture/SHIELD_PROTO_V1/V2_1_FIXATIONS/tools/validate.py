from pathlib import Path
import cadquery as cq,json,hashlib,ast
import trimesh,pymupdf as pdf
P=Path(__file__).resolve().parents[1];rows=[]
for f in (P/'cad').glob('*.step'):
 b=f.read_bytes();s=('\n'.join(l.rstrip()for l in b.decode().splitlines())+'\n').encode();assert b.split()==s.split();f.write_bytes(s);shape=cq.importers.importStep(str(f)).val();assert shape.isValid();rows.append({'file':f.name,'solids':len(shape.Solids()),'valid':True,'sha256':hashlib.sha256(s).hexdigest(),'whitespace_tokens_preserved':True})
m=trimesh.load(P/'cad/D126-support.stl',force='mesh');assert m.is_watertight and m.volume>0
q=json.loads((P/'preuves/geometrie-et-limites.json').read_text());assert len(q['tests'])==9 and all(not c['hits']for c in q['tests']);assert q['tolerance_model']['worst_gap']>.2
v=json.loads((P/'preuves/visserie-ST.json').read_text());unexpected=[h for c in v['cases']for h in c['hits']if h['classification']!='INTENTIONAL_HEAD_TO_PCB_BEARING_NUMERICAL_FACE_TOLERANCE'];assert not unexpected
for f in P.rglob('*'):
 if f.is_file()and f.suffix in ['.json','.md','.py','.scad']:
  b=f.read_text();f.write_text('\n'.join(l.rstrip()for l in b.splitlines())+'\n')
for f in P.rglob('*.py'):ast.parse(f.read_text())
for f in P.rglob('*.json'):json.loads(f.read_text())
with pdf.open(P/'cad/plan-et-coupe.pdf')as d:assert len(d)==1 and d[0].get_text()
res={'status':'PASS_BOUNDED_GEOMETRY_ONLY','STEPs':rows,'STL':{'watertight':True,'volume':float(m.volume),'one_component':len(m.split())==1},'support_cases':9,'support_positive_intersections':0,'fastener_unexpected_intersections':0,'intentional_head_bearing_contacts':v['cases'],'head_contact_explanation':'source PCB to nominal head bearing coincident surface, z extent4.42nm, volume1.458e-5mm3 each; retained and classified by location+depth, no blanket collision threshold raised','worst_gap_mm':q['tolerance_model']['worst_gap'],'PDF':'one page reopened, text verified; envelope diagram, not an actual print','physical':'NOT_TESTED','supplier_complete_quote':False,'no_PCB_CAM_hardware_or_policy_changes':True};(P/'preuves/validation-finale.json').write_text(json.dumps(res,indent=2)+'\n');print(json.dumps({'STEP_count':len(rows),'STL_closed':bool(m.is_watertight),'worst_gap':res['worst_gap_mm'],'unexpected_intersections':0},indent=2))
