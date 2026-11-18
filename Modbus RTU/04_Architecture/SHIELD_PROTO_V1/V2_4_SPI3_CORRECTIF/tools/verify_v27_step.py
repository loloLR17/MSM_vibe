"""Read frozen V2.7 STEP only; compare actual board envelope/drill interfaces.
No model import executes any historical generator or writes the mechanical kit.
"""
from pathlib import Path
from collections import Counter
import json,hashlib
import cadquery as cq
from OCP.BRepAdaptor import BRepAdaptor_Surface
import sexpdata as sx
P=Path(__file__).resolve().parents[1];V27=P.parent/'V2_7_KISS_ABS'
step=V27/'cad/assemblage-complet-enveloppes.step';original=step.read_bytes();assembly=cq.importers.importStep(str(step)).val()
T=lambda n,t:isinstance(n,list)and bool(n)and str(n[0])==t
one=lambda n,t:next(x for x in n if T(x,t))
parts=[]
for solid in assembly.Solids():
 b=solid.BoundingBox()
 if abs(b.xmin-20)<1e-6 and abs(b.xlen-90)<1e-6 and abs(b.ylen-100)<1e-6 and abs(b.zmin-12.25)<1e-6:parts.append(solid)
assert len(parts)==1
shield=parts[0];holes=Counter()
for f in shield.Faces():
 if f.geomType()!='CYLINDER':continue
 b=f.BoundingBox()
 if abs(b.zmin-12.25)>1e-5 or abs(b.zmax-13.85)>1e-5:continue
 cylinder=BRepAdaptor_Surface(f.wrapped).Cylinder();d=cylinder.Axis().Direction();assert abs(d.Z())>.999999
 pt=cylinder.Location();holes[(round(pt.X(),5),round(pt.Y(),5),round(cylinder.Radius(),5))]+=1
native=sx.loads((P/'cad/TR2_Shield_Proto_V1.kicad_pcb').read_text());want=Counter()
for fp in native:
 if not T(fp,'footprint'):continue
 at=one(fp,'at');assert len(at)==3 or float(at[3])==0
 for pad in fp:
  if T(pad,'pad'):
   p=one(pad,'at');dr=one(pad,'drill');assert len(dr)==2
   want[(round(float(at[1])+float(p[1]),5),round(-float(at[2])-float(p[2]),5),round(float(dr[1])/2,5))]+=1
assert holes==want,{'missing':list((want-holes).elements()),'extra':list((holes-want).elements())}
params=json.loads((V27/'sources/parametres.json').read_text());assert params['G']==12.25
# Explicitly preserve the existing 1x10 socket envelopes and J5 stack envelope.
bbs=[]
for solid in assembly.Solids():
 b=solid.BoundingBox();bbs.append([b.xmin,b.xmax,b.ymin,b.ymax,b.zmin,b.zmax])
envelopes={'FRAM_1x10':[43.08,69.78,-60.25,-57.75,13.85,22.35],
           'SD_1x10':[39.25,41.75,-107.92,-81.22,13.85,22.35],
           'J5_frozen_SSW_envelope':[84.66,89.80,-112.685001,-105.005001,3.74,12.25]}
for name,expected in envelopes.items():assert any(max(abs(a-b)for a,b in zip(bb,expected))<1e-5 for bb in bbs),(name,expected)
assert original==step.read_bytes()
result={'verdict':'PASS_FROZEN_V27_STEP_INTERFACE_NUMERICAL','STEP_sha256':hashlib.sha256(original).hexdigest(),'operation':'Read-only STEP import; no generator run, no STL/STEP/PDF/quantity write','assembly_solids':len(bbs),'shield_bbox':[shield.BoundingBox().xmin,shield.BoundingBox().xmax,shield.BoundingBox().ymin,shield.BoundingBox().ymax,shield.BoundingBox().zmin,shield.BoundingBox().zmax],'pad_hole_centres_and_radii_matched':sum(holes.values()),'comparison_resolution_mm':1e-5,'G_mm':12.25,'PCB_thickness_mm':1.6,'frozen_socket_envelopes':envelopes,'vias':'All4 unchanged by separate native M01; small via holes not represented in old envelope STEP','limits':['Old CAD includes passive envelopes and 1x10 sockets; no new physical fit claimed','J5 STEP labels SSW envelope, not proof of actual connector MPN; user later confirms two adapted1x3 sockets','PLA/ABS, Clickv101/v102, contact retention and assembly tolerances remain unqualified physically']}
(P/'preuves/M01-STEP-V27.json').write_text(json.dumps(result,indent=2)+'\n');print(result['verdict'],sum(holes.values()),'holes and3socket envelopes checked')
