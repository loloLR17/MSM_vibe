"""Check nominal model/PCB agreement and exported meshes; never releases physical fit."""
from pathlib import Path
from collections import Counter,defaultdict
import json,subprocess,math,struct,hashlib,re
P=Path(__file__).resolve().parents[1];D=json.loads((P/'design.json').read_text());M=json.loads((P/'mechanical/interfaces.json').read_text());B=json.loads((P/'docs/checks/pcb-readback.json').read_text())
base=json.loads(subprocess.check_output(['git','show','1d2aa6683e972a3751f69229db4022283f55b3fb:Modbus RTU/04_Architecture/SHIELD_PROTO_V1/design.json']))
project=json.loads((P/'cad/TR2_Shield_Proto_V1.kicad_pro').read_text())
rules=project['board']['design_settings']['rules']
assert rules['min_clearance']>=.2 and rules['min_track_width']>=.25 and rules['min_via_diameter']>=.6,'Preserved V1 DRC thresholds were weakened'
assert {c['ref']:c['nets']for c in D['components']}=={c['ref']:c['nets']for c in base['components']},'Unapproved net change'
old={c['ref']:c for c in base['components']}
for c in D['components']:
 if c['ref']!='J7':assert c['xy_mm']==old[c['ref']]['xy_mm'],c['ref']
assert D['mounting_holes_mm']==base['mounting_holes_mm']
scad=(P/'mechanical/retainers.scad').read_text()
for name,owner in [('mki','J8'),('click','J9')]:
 anchors=json.loads(re.search(name+r'_anchors=(\[.*?\]);',scad).group(1))
 assert anchors==[h['xy']for h in D['retainer_mount_holes']if h['owner']==owner],name+' STL source anchors differ'
base_pcb=json.loads(subprocess.check_output(['git','show','1d2aa6683e972a3751f69229db4022283f55b3fb:Modbus RTU/04_Architecture/SHIELD_PROTO_V1/docs/checks/pcb-readback.json']))
oldpads={(q['ref'],q['number']):q for q in base_pcb['pads']if q['number']}
for q in B['pads']:
 if q['number']:
  oldpad=oldpads[q['ref'],q['number']];dx=-3.5 if q['ref']=='J7'else 0
  if q['number']=='2' and q['ref'] in ('C1','C2'):dx={'C1':.04,'C2':-.5}[q['ref']]
  assert abs(q['x']-oldpad['x']-dx)<.0001 and abs(q['y']-oldpad['y'])<.0001,'Unexpected actual pad movement'
assert not any(q['net']in('GND2','+5V','5V')for q in B['pads'])

# Concrete retainer interfaces must match the actual saved PCB, not only the declarative matrix.
for h in D['retainer_mount_holes']:
 assert any(q['npth']and q['ref']==h['owner']and math.dist([q['x'],q['y']],h['xy'])<.001 and abs(q['drill']-h['diameter'])<.001 for q in B['pads']),h
# Positive SD screw heads (5 mm design allowance) do not intersect the card-only swept corridor.
for q in B['pads']:
 if q['ref']=='J7'and q['npth']:
  assert q['x']-q['drill']/2>=20.5
  assert q['y']+2.5<89.5 or q['y']-2.5>102.3,'SD fastener obstructs card path'
# Nominal MKI body is only a hypothesis; this checks internal consistency, not its truth.
def distance_rect(x,y,r):return math.hypot(max(r[0]-x,0,x-r[2]),max(r[1]-y,0,y-r[3]))
for h in D['retainer_mount_holes']:
 for rect in [[42.46,21.46,62.78,54.48],[42.46,56.46,67.86,74.24],[20.18,83.14,43.04,108.54],[83.41,78.73,140.56,104.13]]:
  assert distance_rect(*h['xy'],rect)>3.0,'Nominal pillar intersects a module envelope'
# Automatic keying is waived; no replacement of electrical or mechanical clearance checks.
meshes=[]
for name in ['mki','click','flat_clamp','round_clamp','user_plunger']:
 f=P/f'mechanical/REVIEW_ONLY/{name}.stl';raw=f.read_bytes();n=struct.unpack_from('<I',raw,80)[0];assert len(raw)==84+50*n
 edges=Counter();verts=[];volume=0;parents={}
 def find(x):
  parents.setdefault(x,x)
  if parents[x]!=x:parents[x]=find(parents[x])
  return parents[x]
 for i in range(n):
  tri=[struct.unpack_from('<3f',raw,84+50*i+12+j*12)for j in range(3)];verts+=tri
  a,b,c=tri;volume+=(a[0]*(b[1]*c[2]-b[2]*c[1])+a[1]*(b[2]*c[0]-b[0]*c[2])+a[2]*(b[0]*c[1]-b[1]*c[0]))/6
  pts=[tuple(round(x,5)for x in p)for p in tri]
  for j in range(3):
   edges[tuple(sorted([pts[j],pts[(j+1)%3]]))]+=1
   a0,b0=find(pts[j]),find(pts[(j+1)%3]);parents[a0]=b0
 assert all(v==2 for v in edges.values()),(name,'mesh is not closed')
 assert volume>0,(name,'mesh orientation/volume invalid')
 components=len({find(k)for k in parents});assert components==1,(name,'disconnected solids')
 meshes.append({'name':name,'triangles':n,'closed_edges':True,'connected_surface_components':components,'signed_volume_mm3':volume,'bbox_mm':[[min(x[j]for x in verts),max(x[j]for x in verts)]for j in range(3)],'sha256':hashlib.sha256(raw).hexdigest()})
r={'status':'PASS_NOMINAL_ONLY','electrical_nets_equal_to_V1':True,'nucleo_headers_and_anchors_unchanged':True,'SD_shift_mm':[-3.5,0],'passive_pad_pitch_mm':{'C1':2.54,'C2':2.0},'retainer_holes_match_saved_PCB':True,'nominal_pillar_module_clearance_mm':3.0,'positive_SD_fastener_card_corridor_check':'PASS for 5mm head design allowance and y89.5..102.3 card path','automatic_keying':'NOT_REQUIRED: user decision20261103; no claim of anti-inversion protection','meshes':meshes,'physical_fit':'NOT_VERIFIED','manufacturing_release':'BLOCKED_FABRICATION'}
(P/'docs/checks/mechanical-verification.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps(r,indent=2))
