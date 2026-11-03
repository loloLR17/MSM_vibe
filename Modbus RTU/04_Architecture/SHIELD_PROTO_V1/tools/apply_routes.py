"""Apply candidate paths and copper zones; KiCad DRC is mandatory afterwards."""
from pathlib import Path
import sys,json
import pcbnew as p
P=Path(sys.argv[1]);project_path=P/'cad/TR2_Shield_Proto_V1.kicad_pro';project_bytes=project_path.read_bytes();b=p.LoadBoard(str(P/'cad/TR2_Shield_Proto_V1.kicad_pcb'));r=json.loads((P/'cad/routes.json').read_text());d=json.loads((P/'design.json').read_text());nets={n.GetNetname():n for n in b.GetNetInfo().NetsByNetcode().values()};V=lambda xy:p.VECTOR2I(p.FromMM(xy[0]),p.FromMM(xy[1]))
for route in r['routes']:
 for a,z in zip(route['points'],route['points'][1:]):
  if a['layer']!=z['layer']:
   t=p.PCB_VIA(b);t.SetPosition(V(a['xy']));t.SetWidth(p.FromMM(r['via_diameter_mm']));t.SetDrill(p.FromMM(r['via_drill_mm']));t.SetViaType(p.VIATYPE_THROUGH);t.SetLayerPair(p.F_Cu,p.B_Cu)
  else:
   if a['xy']==z['xy']:continue
   t=p.PCB_TRACK(b);t.SetStart(V(a['xy']));t.SetEnd(V(z['xy']));t.SetLayer(p.F_Cu if a['layer']==0 else p.B_Cu);t.SetWidth(p.FromMM(r['track_width_mm']))
  t.SetNet(nets[route['net']]);b.Add(t)
for layer in [p.F_Cu,p.B_Cu]:
 z=p.ZONE(b);z.SetLayer(layer);z.SetNet(nets['GND']);z.SetLocalClearance(p.FromMM(.25));z.SetThermalReliefGap(p.FromMM(.25));z.SetThermalReliefSpokeWidth(p.FromMM(.3));z.SetMinThickness(p.FromMM(.25));z.SetPadConnection(p.ZONE_CONNECTION_FULL);poly=z.Outline();poly.NewOutline()
 for x,y in d['board_outline_mm']:poly.Append(p.FromMM(x+.5 if x==20 else x-.5),p.FromMM(y+.5 if y==17 else y-.5))
 b.Add(z)
b.BuildConnectivity();p.ZONE_FILLER(b).Fill(b.Zones());p.SaveBoard(str(P/'cad/TR2_Shield_Proto_V1.kicad_pcb'),b);project_path.write_bytes(project_bytes);print('Applied',len(r['routes']),'connections; two GND zones filled')
