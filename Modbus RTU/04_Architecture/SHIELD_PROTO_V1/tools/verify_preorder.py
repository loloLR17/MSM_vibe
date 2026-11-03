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
# The two lower screw heads must clear the enlarged header courtyard;5mm heads fail.
clearances=[]
for ref,h in [('J3',[33.89,97.39]),('J4',[79.61,97.39])]:
 ys=[pcb.ToMM(q.GetPosition().y)for q in fp[ref].Pads()if q.GetNumber()]
 clearance=h[1]-max(ys)-1.52-2.25;assert clearance>0
 clearances.append({'header':ref,'head_and_washer_max_diameter_mm':4.5,'nominal_clearance_to_enlarged_courtyard_mm':round(clearance,6),'C05_real_part':'NOT_VERIFIED'})
project=json.loads((P/'cad/TR2_Shield_Proto_V1.kicad_pro').read_text());rules=project['board']['design_settings']['rules']
assert rules['min_clearance']==.2 and rules['min_track_width']==.25 and rules['min_via_diameter']==.6
# Compare semantic routes, not randomized KiCad UUID/order.
old=json.loads((P/'docs/checks/v12/routes-baseline.json').read_text())
new=json.loads((P/'cad/routes.json').read_text());changed=[i for i,(a,b)in enumerate(zip(old['routes'],new['routes']))if a!=b]
assert len(old['routes'])==len(new['routes'])==32 and changed==[21]
assert new['routes'][21]['net']=='+3V3'
result={'status':'PASS_NOMINAL_CAD_ONLY','bottom_headers':['J1','J2','J3','J4','J5'],'J5_69_PG4':'RTU_DE','J5_70_PG6':'NC','header_drill_mm':1.02,'passive_pitch_mm':{'C1':2.54,'C2':2.0},'screw_clearances':clearances,'routes_retained':31,'route_changed':'TP1.1-C2.1 +3V3 only','strict_rules':{k:rules[k]for k in ['min_clearance','min_track_width','min_via_diameter']},'C05_manufacturer_CAD':'NOT_RECEIVED','physical_mating':'NOT_VERIFIED'}
(P/'docs/checks/preorder-native.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
