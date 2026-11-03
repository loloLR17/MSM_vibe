"""Native pcbnew check of bottom mating headers, selected-part geometry and local changes.
Run with KiCad Python and the dossier directory argument. No C05 parity claim.
"""
from pathlib import Path
from collections import Counter
import sys,json,math,subprocess
import pcbnew as pcb
P=Path(sys.argv[1]);D=json.loads((P/'design.json').read_text());B=pcb.LoadBoard(str(P/'cad/TR2_Shield_Proto_V1.kicad_pcb'))
fp={f.GetReference():f for f in B.GetFootprints()};pads={(f.GetReference(),q.GetNumber()):q for f in B.GetFootprints()for q in f.Pads()if q.GetNumber()}
for c in D['components']:
 f=fp[c['ref']];assert f.GetLayer()==(pcb.B_Cu if c['ref']in('J1','J2','J3','J4','J5')else pcb.F_Cu)
 for n,net in c['nets'].items():assert pads[c['ref'],n].GetNetname()==net
 for q in f.Pads():
  if q.GetNumber()and c['ref'].startswith('J'):assert abs(pcb.ToMM(q.GetDrillSize().x)-1.02)<1e-6
for ref,pitch in [('C1',2.54),('C2',2.0)]:
 assert abs(pcb.ToMM((pads[ref,'2'].GetPosition()-pads[ref,'1'].GetPosition()).x)-pitch)<1e-6
assert pads['J5','1'].GetNetname()=='RTU_DE'
assert pads['J5','2'].GetNetname().startswith('unconnected-')
assert abs(pcb.ToMM(pads['J5','1'].GetPosition().x)-85.96)<1e-6
assert abs(pcb.ToMM(pads['J5','2'].GetPosition().x)-88.5)<1e-6
# Spacers occupy the gap between boards. Screw heads sit outside the two PCBs.
# Check both: conservative spacer OD4 against male body/courtyard and top head OD4.5 against solder pads.
clearances=[]
for ref,h in [('J3',[33.89,97.39]),('J4',[79.61,97.39])]:
 ys=[pcb.ToMM(q.GetPosition().y)for q in fp[ref].Pads()if q.GetNumber()]
 clearance=h[1]-max(ys)-1.77-2.0;assert clearance>0
 for pad in fp[ref].Pads():
  if pad.GetNumber():assert math.dist(h,[pcb.ToMM(pad.GetPosition().x),pcb.ToMM(pad.GetPosition().y)])-2.25-.85>.2
 clearances.append({'header':ref,'head_and_washer_max_diameter_mm':4.5,'spacer_max_diameter_mm':4.0,'nominal_spacer_clearance_to_enlarged_courtyard_mm':round(clearance,6),'head_underboard_overlap':'not coplanar; outside shield top and Nucleo underside','C05_real_part':'NOT_VERIFIED'})
project=json.loads((P/'cad/TR2_Shield_Proto_V1.kicad_pro').read_text());rules=project['board']['design_settings']['rules']
assert rules['min_clearance']==.2 and rules['min_track_width']==.25 and rules['min_via_diameter']==.6
# Compare semantic routes, not randomized KiCad UUID/order.
old=json.loads((P/'docs/checks/v13/routes-baseline.json').read_text())
new=json.loads((P/'cad/routes.json').read_text());changed=[i for i,(a,b)in enumerate(zip(old['routes'],new['routes']))if a!=b]
assert len(old['routes'])==len(new['routes'])==32 and changed==[]
assert fp['J5'].GetValue()=='CN12_69_70_PG4_FEMALE'
assert 'FEMALE' in fp['J5'].GetLibDescription()
assert any(g.GetLayer()==pcb.B_Fab for g in fp['J5'].GraphicalItems())
points=[pt for g in fp['J5'].GraphicalItems()if g.GetLayer()==pcb.B_Fab for pt in (g.GetStart(),g.GetEnd())]
bounds=[min(pcb.ToMM(pt.x)for pt in points),min(pcb.ToMM(pt.y)for pt in points),max(pcb.ToMM(pt.x)for pt in points),max(pcb.ToMM(pt.y)for pt in points)]
assert all(abs(a-b)<1e-6 for a,b in zip(bounds,[84.44,110.09,90.02,112.63]))
bottom_text=[x for x in B.GetDrawings()if isinstance(x,pcb.PCB_TEXT)and x.GetLayer()==pcb.B_SilkS]
assert any('J5 FEM' in x.GetText()and x.IsMirrored()for x in bottom_text)
result={'status':'PASS_NOMINAL_CAD_ONLY','bottom_headers':['J1','J2','J3','J4','J5'],'J5_69_PG4':'RTU_DE','J5_70_PG6':'NC','header_drill_mm':1.02,'passive_pitch_mm':{'C1':2.54,'C2':2.0},'screw_clearances':clearances,'routes_retained':32,'route_changed':None,'J5_gender':'FEMALE_BOTTOM','J5_actual_B_Fab_bounds_mm':bounds,'bottom_J5_silk_is_mirrored':True,'J5_body_height_nominal_mm':8.5,'Nucleo_modification':'NONE_USER_CONFIRMED_EXISTING_MALES','strict_rules':{k:rules[k]for k in ['min_clearance','min_track_width','min_via_diameter']},'C05_manufacturer_CAD':'NOT_RECEIVED','physical_mating':'NOT_VERIFIED'}
(P/'docs/checks/preorder-native.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
