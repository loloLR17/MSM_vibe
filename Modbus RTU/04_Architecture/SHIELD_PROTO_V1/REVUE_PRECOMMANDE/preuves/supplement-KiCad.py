from pathlib import Path
import sys,json,math,pcbnew as pcb
p=Path(sys.argv[1]);b=pcb.LoadBoard(str(p/'cad/TR2_Shield_Proto_V1.kicad_pcb'));m=pcb.ToMM
fp=[]
for f in b.GetFootprints():
 fp.append({'ref':f.GetReference(),'attributes':f.GetAttributes(),'courtyard_graphics':sum(g.GetLayer() in [pcb.F_CrtYd,pcb.B_CrtYd] for g in f.GraphicalItems()),'numbered_pad_count':sum(bool(q.GetNumber()) for q in f.Pads())})
via=[t for t in b.GetTracks() if t.GetClass()=='PCB_VIA'];off=[]
for v in via:
 for t in b.GetTracks():
  if t.GetClass()=='PCB_VIA' or t.GetNetCode()!=v.GetNetCode():continue
  for pos in [t.GetStart(),t.GetEnd()]:
   dist=math.hypot(m(pos.x-v.GetPosition().x),m(pos.y-v.GetPosition().y))
   if dist< m(v.GetWidth(t.GetLayer()))/2 and dist>1e-6:off.append({'net':t.GetNetname(),'offset_mm':dist,'layer':t.GetLayerName(),'endpoint':[m(pos.x),m(pos.y)],'via_center':[m(v.GetPosition().x),m(v.GetPosition().y)]})
r={'footprints':fp,'missing_courtyards':[v['ref'] for v in fp if not v['courtyard_graphics']],'track_ends_touching_via_not_centered':off,'notes':'supplement to default ignored KiCad checks; no project settings changed'}
(p/'REVUE_PRECOMMANDE/preuves/supplement-KiCad.json').write_bytes((json.dumps(r,indent=2)+'\n').encode());print(json.dumps(r));assert not off, 'optional centered-via style check failed; findings preserved'
