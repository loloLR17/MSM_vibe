import pcbnew as pcb,sys,json
from pathlib import Path
p=Path(sys.argv[1]);print([x for x in dir(pcb.PCB_IO_MGR) if 'ALTIUM' in x or 'Load' in x],flush=True)
plugin=pcb.PCB_IO_MGR.FindPlugin(pcb.PCB_IO_MGR.ALTIUM_DESIGNER)
b=plugin.LoadBoard(str(p/'ST-design/MB1549C/MB1549.PcbDoc'),None)
print('loaded',len(list(b.GetFootprints())),flush=True)
rows=[]
for f in b.GetFootprints():
 row={'ref':f.GetReference(),'value':f.GetValue(),'xy':[pcb.ToMM(f.GetPosition().x),pcb.ToMM(f.GetPosition().y)],'angle':f.GetOrientationDegrees(),'side':f.GetLayerName(),'pads':[]}
 for x in f.Pads():row['pads'].append({'number':x.GetNumber(),'xy':[pcb.ToMM(x.GetPosition().x),pcb.ToMM(x.GetPosition().y)],'net':x.GetNetname(),'drill':[pcb.ToMM(x.GetDrillSize().x),pcb.ToMM(x.GetDrillSize().y)]})
 rows.append(row)
(p/'ST-native-import.json').write_text(json.dumps({'footprints':rows,'texts':[x.GetText() for x in b.GetDrawings() if isinstance(x,pcb.PCB_TEXT)]},indent=2)+'\n')
pcb.SaveBoard(str(p/'ST-native-import.kicad_pcb'),b)
