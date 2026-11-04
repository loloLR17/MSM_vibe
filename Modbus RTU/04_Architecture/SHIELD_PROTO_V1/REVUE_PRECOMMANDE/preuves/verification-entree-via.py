from pathlib import Path
import sys,json,math,pcbnew as pcb
p=Path(sys.argv[1]);b=pcb.LoadBoard(str(p/'cad/TR2_Shield_Proto_V1.kicad_pcb'));m=pcb.ToMM;result=[]
for v in b.GetTracks():
 if v.GetClass()!='PCB_VIA':continue
 row={'net':v.GetNetname(),'via_center':[m(v.GetPosition().x),m(v.GetPosition().y)],'layers':{}}
 for layer in [pcb.F_Cu,pcb.B_Cu]:
  centered=[]
  for t in b.GetTracks():
   if t.GetClass()=='PCB_VIA' or t.GetNetCode()!=v.GetNetCode() or t.GetLayer()!=layer:continue
   def xy(pos):return [m(pos.x),m(pos.y)]
   if any(math.dist(xy(pos),row['via_center'])<1e-6 for pos in [t.GetStart(),t.GetEnd()]):centered.append({'start':xy(t.GetStart()),'end':xy(t.GetEnd()),'length_mm':m(t.GetLength()),'width_mm':m(t.GetWidth())})
  assert centered,(row,layer)
  row['layers'][b.GetLayerName(layer)]=centered
 result.append(row)
assert len(result)==4
r={'status':'PASS','criterion':'each saved through-via has same-net track terminating on its center on both copper layers; intermediate vertices inside the via pad are not required to coincide with center','vias':result,'initial_probe_failure_explained':'ACC_SCK intermediate vertex49.9/37.5 is connected by0.1mm B.Cu segment to actual via center49.8/37.5; no PCB change','no_SaveBoard':True}
(p/'REVUE_PRECOMMANDE/preuves/verification-entree-via.json').write_bytes((json.dumps(r,indent=2)+'\n').encode());print(json.dumps(r))
