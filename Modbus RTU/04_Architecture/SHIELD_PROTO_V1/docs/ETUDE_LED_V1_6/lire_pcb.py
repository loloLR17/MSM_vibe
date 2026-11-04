from pathlib import Path
import sys,json
import pcbnew as pcb
P=Path(sys.argv[1]);b=pcb.LoadBoard(str(P/'cad/TR2_Shield_Proto_V1.kicad_pcb'));m=pcb.ToMM
r={'scope':'read-only pcbnew.LoadBoard; no SaveBoard','pads':[],'tracks':[],'zones':len(b.Zones())}
for f in b.GetFootprints():
 for a in f.Pads():r['pads'].append({'ref':f.GetReference(),'number':a.GetNumber(),'net':a.GetNetname(),'xy':[m(a.GetPosition().x),m(a.GetPosition().y)],'diameter_mm':m(a.GetSize().x),'drill_mm':m(a.GetDrillSize().x)})
for t in b.GetTracks():
 if t.GetClass()=='PCB_VIA':continue
 r['tracks'].append({'net':t.GetNetname(),'layer':b.GetLayerName(t.GetLayer()),'start':[m(t.GetStart().x),m(t.GetStart().y)],'end':[m(t.GetEnd().x),m(t.GetEnd().y)],'width_mm':m(t.GetWidth())})
(P/'docs/ETUDE_LED_V1_6/preuves/pcb_natif_lecture_seule.json').write_bytes((json.dumps(r,indent=2)+'\n').encode('utf-8'));print('Read-only extracted board:',len(r['pads']),'pads',len(r['tracks']),'segments',r['zones'],'zones')
