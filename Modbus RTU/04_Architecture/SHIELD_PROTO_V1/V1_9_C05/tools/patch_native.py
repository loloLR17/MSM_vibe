from pathlib import Path
import sys,json,math
import pcbnew as pcb
P=Path(sys.argv[1]);D=json.loads((P/'design.json').read_text());pr=P/'cad/TR2_Shield_Proto_V1.kicad_pro';original=pr.read_bytes();b=pcb.LoadBoard(str(P/'cad/TR2_Shield_Proto_V1.kicad_pcb'));f={x.GetReference():x for x in b.GetFootprints()};v=lambda xy:pcb.VECTOR2I(round(xy[0]*1e6),round(xy[1]*1e6))
for c in D['components']:
 if c['ref']in('J1','J5'):f[c['ref']].SetPosition(v(c['xy_mm']))
for pad in f['J1'].Pads():
 if pad.GetAttribute()==pcb.PAD_ATTRIB_NPTH:
  xy=[pcb.ToMM(pad.GetPosition().x),pcb.ToMM(pad.GetPosition().y)];target=min(D['mounting_holes_mm'],key=lambda h:math.dist(h,xy));pad.SetPosition(v(target))
for x in b.GetDrawings():
 if isinstance(x,pcb.PCB_TEXT):
  if x.GetText()=='J1':x.SetPosition(v([80.34,24.016]))
  if 'SHIELD V1.4' in x.GetText():x.SetText(x.GetText().replace('V1.4','V1.9'))
# Local socket geometry unchanged; J1 mount-hole relative offsets need updated library matching board.
pcb.PCB_IO_MGR.FindPlugin(pcb.PCB_IO_MGR.KICAD_SEXP).FootprintSave(str(P/'cad/TR2.pretty'),f['J1'])
# Correct tail endpoints first; authoritative route replacement follows local routing.
r=json.loads((P/'cad/routes.json').read_text())
for z in r['routes']:
 if z['to']=='J5.5':
  z['points'][-1]['xy']=[85.96,111.385001]
(P/'cad/routes.json').write_text(json.dumps(r,indent=2)+'\n')
# Remove copper only in this candidate; reapply retained routes and local replacements together.
result={'outline':D['board_outline_mm'],'pads':[],'footprints':[]}
for fp in b.GetFootprints():
 result['footprints'].append({'ref':fp.GetReference(),'value':fp.GetValue(),'position':[pcb.ToMM(fp.GetPosition().x),pcb.ToMM(fp.GetPosition().y)],'side':'B'if fp.GetLayer()==pcb.B_Cu else'F'})
 for pad in fp.Pads():result['pads'].append({'ref':fp.GetReference(),'number':pad.GetNumber(),'net':pad.GetNetname(),'x':pcb.ToMM(pad.GetPosition().x),'y':pcb.ToMM(pad.GetPosition().y),'diameter':pcb.ToMM(pad.GetSize().x),'drill':pcb.ToMM(pad.GetDrillSize().x),'npth':pad.GetAttribute()==pcb.PAD_ATTRIB_NPTH})
pcb.SaveBoard(str(P/'cad/TR2_Shield_Proto_V1.kicad_pcb'),b);pr.write_bytes(original);(P/'docs/pcb_geometry.json').write_text(json.dumps(result,indent=2)+'\n');print('Candidate socket corrections applied, no modules/outline/schema changed')
