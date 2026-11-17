from pathlib import Path
import pcbnew as pcb,sys,json,hashlib
source=Path(sys.argv[1]);out=Path(sys.argv[2]);board=pcb.LoadBoard(str(source));pads=[]
for f in board.GetFootprints():
 if f.GetReference() in ['J2','J8','TP3']:
  for q in f.Pads():pads.append({'ref':f.GetReference(),'pin':q.GetNumber(),'net':q.GetNetname(),'x_mm':pcb.ToMM(q.GetPosition().x),'y_mm':pcb.ToMM(q.GetPosition().y)})
meta={'source_file':str(source),'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),'method':'native KiCad LoadBoard/Pads/GetNetname; read only, no Save/fill/export/mutation','pads':pads};out.write_text(json.dumps(meta,indent=2)+'\n')
for x in pads:
 if x['ref']=='J8' and x['pin']in['19','20','21','22','23','24'] or x['ref']=='TP3':print(x)
