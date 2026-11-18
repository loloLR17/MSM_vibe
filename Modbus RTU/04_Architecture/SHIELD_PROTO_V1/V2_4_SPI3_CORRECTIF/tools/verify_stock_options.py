"""Stock assembly bounds versus frozen V2.7; no electronic/3D source writes.
Nominal envelope/contact feasibility only, not actual insertion/retention evidence.
"""
from pathlib import Path
import json
import cadquery as cq
P=Path(__file__).resolve().parents[1];V27=P.parent/'V2_7_KISS_ABS'
solid=cq.importers.importStep(str(V27/'cad/assemblage-complet-enveloppes.step')).val();allparts=solid.Solids()
def bb(s):
 b=s.BoundingBox();return[b.xmin,b.xmax,b.ymin,b.ymax,b.zmin,b.zmax]
def equal(a,b):return max(abs(x-y)for x,y in zip(a,b))<1e-5
def box(b):return cq.Solid.makeBox(b[1]-b[0],b[3]-b[2],b[5]-b[4],cq.Vector(b[0],b[2],b[4]))
def overlap(a,b):return all(min(a[i+1],b[i+1])-max(a[i],b[i])>1e-6 for i in(0,2,4))
frozen_j5=[84.66,89.8,-112.685001,-105.005001,3.74,12.25]
# Full manufacturer Samtec body is slightly longer than the old simplified STEP.
samtec=[84.755,89.705,-112.910001,-104.780001,3.74,12.25]
adapted=[84.71,89.75,-112.905001,-104.785001,3.75,12.25]
checks=[]
for name,b in [('SSW_103_01_F_D_manufacturer_nominal_body',samtec),('user_two_1x3_nominal_combined_bounds_not_contact_qualification',adapted)]:
 probe=box(b);collisions=[];tested=0
 for i,s in enumerate(allparts):
  other=bb(s)
  if equal(other,frozen_j5):continue # replace own simplified envelope only in memory
  if overlap(b,other):
   tested+=1;volume=abs(probe.intersect(s).Volume())
   if volume>1e-5:collisions.append({'solid_index':i,'bbox':other,'volume_mm3':volume})
 assert not collisions,(name,collisions)
 checks.append({'name':name,'bbox':b,'positive_collisions':collisions,'overlapping_bbox_shapes_evaluated':tested})
# Stock1x10 intact housings are already explicitly included in V2.7 assembly.
frozen={'FRAM':[43.08,69.78,-60.25,-57.75,13.85,22.35],'SD':[39.25,41.75,-107.92,-81.22,13.85,22.35]}
actual={'FRAM':[43.48,69.38,-60.25,-57.75,13.85,22.35],'SD':[39.25,41.75,-107.52,-81.62,13.85,22.35]}
for name,b in actual.items():
 env=frozen[name];assert any(equal(bb(s),env)for s in allparts)
 assert all(env[i]-1e-6<=b[i] and b[i+1]<=env[i+1]+1e-6 for i in(0,2,4))
# Source manufacturer dimensions versus conservative unchanged passive envelopes.
assert all(a<=b for a,b in zip([5.08,3.175,5.08],[6,4,7]))
assert all(a<=b for a,b in zip([5,5,11],[6,6,14]))
result={'status':'PASS_NOMINAL_STOCK_OPTIONS_WITH_LIMITS','J5_body_extension_beyond_simplified_STEP_y_mm':.225,'J5_explanation':'Old STEP box7.68mm long is smaller than actual nominal8.13mm Samtec; not hidden or repaired. Full body checked against frozen other parts in memory with0positive intersections. Fixed PCB and all STL/STEP remain byteexact.','J5_collision_checks':checks,'Samtec_nominal_contact_budget':{'male_tip_height_mm':8.22,'G_mm':12.25,'body_height_mm':8.51,'insertion_mm':4.48,'manufacturer_insertion_range_mm':[3.68,6.35],'within_range':True,'tolerance_budget':'Reuse unchanged V2.0 source proof; actual body/post/jumper assembly still requires cold validation'},'J6_J7_1x10_body_bounds':actual,'J6_J7_frozen_bounds':frozen,'extra_contact10':'Must be neutralized before eventual assembly to prevent contact with PCB; no trace/pad/hole added, no physical operation requested/executed in this mission','C1_SR205_max_body_mm':[5.08,3.175,5.08],'C1_frozen_envelope_mm':[6,4,7],'C1_native_courtyard_reserved_issue':'Existing native courtyard narrower than AVX body by0.1875mm per side; frozen mechanical envelope explicitly already covers AVX, no silent footprint alteration. Leads/standoff/actual fit remain cold gate.','C2_EEUFR1H100_max_body_mm':[5,5,11],'C2_frozen_envelope_mm':[6,6,14],'J5_adapted_Connfly_contact_depth':'UNKNOWN. Physical previous RS485 result is valid for that assemblage, not Samtec presence or mechanical retention qualification. Not adopted as equivalent final MPN.','limits':'Nominal vendor/old frozen-envelope model only; material, dimensional production tolerances, contact force, soldered assembly and PLA fit not qualified'}
(P/'preuves/stock-options-M01.json').write_text(json.dumps(result,indent=2)+'\n');print('Stock options nominally fit; full J5 body0collisions; no Samtec equivalence claimed for adapted1x3')
