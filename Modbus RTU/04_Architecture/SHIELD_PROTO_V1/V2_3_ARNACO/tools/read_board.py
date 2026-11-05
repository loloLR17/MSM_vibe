"""Independent readback of saved PCB using native KiCad Python."""
from pathlib import Path
import sys,json
import pcbnew as p
P=Path(sys.argv[1]);b=p.LoadBoard(str(P/'cad/TR2_Shield_Proto_V1.kicad_pcb'));r={'pads':[],'tracks':[]}
for f in b.GetFootprints():
 for q in f.Pads():r['pads'].append({'ref':f.GetReference(),'number':q.GetNumber(),'net':q.GetNetname(),'x':p.ToMM(q.GetPosition().x),'y':p.ToMM(q.GetPosition().y),'diameter':p.ToMM(q.GetSize().x),'drill':p.ToMM(q.GetDrillSize().x),'npth':q.GetAttribute()==p.PAD_ATTRIB_NPTH})
for t in b.GetTracks():r['tracks'].append({'net':t.GetNetname(),'layer':t.GetLayerName(),'via':type(t).__name__=='PCB_VIA','x':p.ToMM(t.GetPosition().x),'y':p.ToMM(t.GetPosition().y),'length_mm':p.ToMM(t.GetLength())})
(P/'docs/checks/pcb-readback.json').write_text(json.dumps(r,indent=2)+'\n')
