"""Additional checks only: taller, properly sourced shield passives and LED paths.

Printed geometry unchanged after controle.py pass. Do not repeat acquired mesh tests.
"""
import json,math,hashlib
import cadquery as cq
import modele as v
import controle as c
P=v.P;m=v.m
v.build();rows=[];bad=[]
def check(n,a,b):
 vol=c.volume(a,b);r={'check':n,'positive_volume_mm3':vol};rows.append(r)
 if vol>1e-5:bad.append(r)
rc=c.passive_envelopes();hw=cq.importers.importStep(str(P/'cad/quincaillerie-enveloppes.step')).val()
for name,s in rc:
 for r in v.INST:check(name+'/'+r['label'],s,r['shape'])
 check(name+'/all_checked_metal_washers',s,hw)
 for which in ['FRAM','SD']:
  for H in [10,10.6,11.2,11.8]:check(name+f'/saddle{which}{H}',s,v.saddle(which,H))
 for name2,s2 in c.refs():
  if name2!='shield_actual_holes_RC_envelopes':check(name+'/'+name2,s,s2)
# New added geometry stays clear of the original1060 ST source boxes.
meta=json.loads((P.parent/'V2_0_EMBOITEMENT/preuves/C05-collisions-G12p25.json').read_text());tx,ty=meta['Tx_Ty'];nb=[]
for r in meta['source_geometry']:
 b=r['bbox_ST'];nb.append([b[0]+tx,b[1]+tx,b[2]-ty,b[3]-ty,b[4],b[5]])
for H in [8.56,10.2]:
 for stroke in [0,.7]:
  s=v.pusher(H+.3).translate((0,0,-stroke));b=c.bb(s)
  hits=[q for q in nb if c.near(b,q)]
  for j,bx in enumerate(hits):check(f'USER calibratedH{H} stroke{stroke}/STbbox{j}',s,cq.Solid.makeBox(bx[1]-bx[0],bx[3]-bx[2],bx[5]-bx[4],cq.Vector(bx[0],bx[2],bx[4])))
# Retest existing common CAD observers against the actual new opaque own shapes.
old=json.loads((P.parent/'V2_4_IMPRESSION_3D/preuves/optique-bornage.json').read_text());obs=[]
for o in old['clear_common_CAD_observers']:
 blockers=[]
 for ray in o['rays']:
  a=o['observer'];b=ray['CAD_target'];delta=[b[i]-a[i]for i in range(3)];L=math.sqrt(sum(x*x for x in delta));probe=cq.Solid.makeCylinder(.08,L,cq.Vector(*a),cq.Vector(*[x/L for x in delta]))
  for r in v.INST:
   if c.volume(probe,r['shape'])>1e-5:blockers.append([ray['LED'],r['label']])
 obs.append({'observer':o['observer'],'all_three_clear_against_V27_opaque_parts':not blockers,'blockers':blockers})
assert any(r['all_three_clear_against_V27_opaque_parts']for r in obs)
# Restore hatch closure and logo identity checked by volumes, no general redesign.
lid=v.PARTS['02_couvercle_ARNACO_USER']['shape'];closed=m.box(131,78,47,145,107,50)
assert abs(c.volume(lid,closed)-closed.Volume())<1e-5
slice=m.box(8,-20,50.0001,166,146,52);oldlogo=v.ref('02_couvercle_ARNACO').intersect(slice);newlogo=lid.intersect(slice);assert abs(newlogo.cut(oldlogo).Volume())+abs(oldlogo.cut(newlogo).Volume())<1e-5
proof={'passive_envelope_checks':rows,'positive_intersections':bad,'passive_dims_C1':[6,4,7],'passive_dims_C2':[6,6,14],'resistor_body_envelope':[4.5,2.5,3.5],'placement':'centres computed from actual pads, not uniform footprint origin assumption','C2_source_nominal_D5_H11':'V22/sourcePanasonic; envelopeD6H14 includes standoff','C2_nominal_gap_to_full_SD_female_end':81.22-(78+2.5),'C2_design_envelope_gap_to_full_SD_female_end':81.22-(78+3),'LED_existing_observers':len(obs),'LED_common_still_clear':sum(r['all_three_clear_against_V27_opaque_parts']for r in obs),'LED_limit':'geometry only; emission pattern/window/real cap position not physically qualified','former_RS485_roof_hatch_closed':True,'ARNACO_above_Z50_symmetric_difference':0,'mesh_tests_reused':'immutable existing STL SHA verified below','passed':not bad}
for q in json.loads((P/'preuves/pieces.json').read_text()):assert hashlib.sha256((P/q['STL']).read_bytes()).hexdigest()==q['STL_sha256']
(P/'preuves/supplement-controles.json').write_text(json.dumps(proof,indent=2,ensure_ascii=False)+'\n');(P/'preuves/LED-V27.json').write_text(json.dumps(obs,indent=2)+'\n');print(json.dumps({k:val for k,val in proof.items()if k!='passive_envelope_checks'},indent=2),flush=True);assert not bad
