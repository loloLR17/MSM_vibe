"""KiCad 10 pcbnew generator. Run with KiCad's Python, not on an MCU.
The connection matrix is authoritative; mechanical release gates remain explicit.
"""
from pathlib import Path
import json,sys,math
import pcbnew as pcb
P=Path(sys.argv[1]).resolve();D=json.loads((P/'design.json').read_text());CAD=P/'cad';LIB=CAD/'TR2.pretty';LIB.mkdir(exist_ok=True)
project_path=CAD/'TR2_Shield_Proto_V1.kicad_pro'
project_bytes=project_path.read_bytes() # pcbnew.SaveBoard can rewrite loaded/default project settings
b=pcb.BOARD();b.SetCopperLayerCount(2)
MM=pcb.FromMM
V=lambda x,y:pcb.VECTOR2I(MM(x),MM(y))
N={}
for name in sorted({n for c in D['components']for n in c['nets'].values()}):
 n=pcb.NETINFO_ITEM(b,name);b.Add(n);N[name]=n

def line(parent,a,z,layer,width=0.15):
 s=pcb.PCB_SHAPE(parent);s.SetShape(pcb.SHAPE_T_SEGMENT);s.SetStart(V(*a));s.SetEnd(V(*z));s.SetLayer(layer);s.SetWidth(MM(width));parent.Add(s);return s

def rect(parent,x0,y0,x1,y1,layer,width=0.15):
 for a,z in [((x0,y0),(x1,y0)),((x1,y0),(x1,y1)),((x1,y1),(x0,y1)),((x0,y1),(x0,y0))]:line(parent,a,z,layer,width)

def pad(fp,num,x,y,net='',diameter=1.7,drill=1.0,npth=False):
 p=pcb.PAD(fp);p.SetNumber(str(num));p.SetShape(pcb.PAD_SHAPE_CIRCLE);p.SetAttribute(pcb.PAD_ATTRIB_NPTH if npth else pcb.PAD_ATTRIB_PTH);p.SetSize(V(diameter,diameter));p.SetDrillSize(V(drill,drill));p.SetLayerSet(pcb.PAD.PTHMask());p.SetPosition(V(x,y))
 if not net and str(num) and not npth:
  net='unconnected-('+fp.GetReference()+'-NC-Pad'+str(num)+')'
  if net not in N:
   n=pcb.NETINFO_ITEM(b,net);b.Add(n);N[net]=n
 if net:p.SetNet(N[net])
 fp.Add(p);return p

def local_pads(c):
 kind=c['kind'];n=c['n']
 if kind=='dual':return [(str(2*j+i+1),i*2.54,j*2.54)for j in range(n)for i in range(2)]
 if kind=='testpoint':return [('1',0,0)]
 if kind=='single':return [(str(i+1),i*2.54,0)for i in range(n)]
 if kind=='fram':return [(str(i+1),i*2.54,0)for i in range(n)]
 if kind=='sd':return [(str(i+1),0,-i*2.54)for i in range(n)]
 if kind=='dil':return [(str(i+1),0,i*2.54)for i in range(n)]+[(str(2*n-i),c['row_pitch_mm'],i*2.54)for i in range(n)]
 if kind=='click':return [(str(i+1),-i*2.54,0)for i in range(n)]+[(str(2*n-i),-i*2.54,22.86)for i in range(n)]
 if kind=='resistor':return [('1',-3.81,0),('2',3.81,0)]
 if kind=='capacitor':return [('1',0,0),('2',c.get('lead_pitch_mm',2.5),0)]
 raise ValueError(kind)

for c in D['components']:
 f=pcb.FOOTPRINT(b);f.SetReference(c['ref']);f.SetValue(c['value']);f.SetFPID(pcb.LIB_ID('TR2',c['ref']+'_'+c['value']));f.SetAttributes(pcb.FP_THROUGH_HOLE)
 path=pcb.KIID_PATH();path.push_back(pcb.KIID(D['root_uuid']));path.push_back(pcb.KIID(c['uuid']));f.SetPath(path)
 side=pcb.B_Cu if c.get('assembly_side')=='BOTTOM'else pcb.F_Cu;f.SetLayer(side);court=pcb.B_CrtYd if side==pcb.B_Cu else pcb.F_CrtYd
 pts=local_pads(c)
 for num,x,y in pts:pad(f,num,x,y,c['nets'].get(num,''),drill=0.8 if c['kind']in('resistor','capacitor')else (1.0 if c['kind']=='testpoint' else 1.02))
 xs=[x for _,x,y in pts];ys=[y for _,x,y in pts];box=[min(xs)-1.4,min(ys)-1.4,max(xs)+1.4,max(ys)+1.4]
 if c['kind']=='fram':
  rect(f,-2.54,-2.54,22.86,15.24,pcb.F_Fab)
  for x in [0,20.32]:pad(f,'',x,12.7,diameter=2.7,drill=2.7,npth=True)
  box=[-2.8,-2.8,23.1,15.5]
 if c['kind']=='sd':
  rect(f,-20.32,-22.86,2.54,2.54,pcb.F_Fab)
  for y in [0,-20.32]:pad(f,'',-17.78,y,diameter=2.7,drill=2.7,npth=True)
  box=[-20.6,-23.1,2.8,2.8]
 if c['kind']=='dil':
  rect(f,-2.54,-2.54,17.78,30.48,pcb.F_Fab);box=[-2.8,-2.8,18.0,30.7]
 if c['kind']=='click':
  # 57.15x25.4 manufacturer envelope, 22.86 row spacing. Long isolated end projects beyond carrier.
  rect(f,-21.59,-1.27,35.56,24.13,pcb.F_Fab);box=[-21.84,-1.52,35.81,24.38]
 if c['kind']=='capacitor':
  if c['ref']=='C2':
   rect(f,-1.75,-2.75,3.75,2.75,pcb.F_Fab);box=[-2,-3,4,3]
  else:rect(f,-1.3,-2.5,3.8,2.5,pcb.F_Fab)
 # Courtyard describes the on-carrier socket, not the elevated module envelope.
 # Module outline remains F.Fab; the 11 mm elevation and clipped tails need fit verification.
 if c['kind']in('fram','sd','dil','click'):
  if c['kind']=='fram':rect(f,-1.92,-1.645,22.24,1.645,court,.05)
  elif c['kind']=='sd':rect(f,-1.575,-22.24,1.645,1.90,court,.05)
  elif c['kind']=='dil':
   for x in [0,15.24]:rect(f,x-1.645,-1.92,x+1.645,29.86,court,.05)
  else:
   for y in [0,22.86]:rect(f,-19.70,y-1.645,1.92,y+1.645,court,.05)
 else:
  if c['kind']=='dual':box=[min(xs)-1.605,min(ys)-1.77,max(xs)+1.605,max(ys)+1.77]
  if c['ref']=='J5':
   box=[-1.92,-1.645,4.46,1.645]
   rect(f,-1.52,-1.27,4.06,1.27,pcb.B_Fab)
   # Female receptacle entry outline and pin1 on underside; XY/pad numbering never flipped.
   for xx in [0,2.54]:rect(f,xx-.45,-.45,xx+.45,.45,pcb.B_Fab)
   f.SetLibDescription('J5 FEMALE DS1023-1*2S21 bottom; receive existing Nucleo69/70 male; no Nucleo modification')
  rect(f,*box,court,0.05)
 if c['ref']=='J1':
  ox,oy=c['xy_mm']
  for x,y in D['mounting_holes_mm']:pad(f,'',x-ox,y-oy,diameter=D['mounting_hole_diameter_mm'],drill=D['mounting_hole_diameter_mm'],npth=True)
 if c['ref']in('J8','J9'):
  ox,oy=c['xy_mm']
  for h in D['retainer_mount_holes']:
   if h['owner']==c['ref']:pad(f,'',h['xy'][0]-ox,h['xy'][1]-oy,diameter=h['diameter'],drill=h['diameter'],npth=True)
 f.Reference().SetVisible(False);f.Value().SetVisible(False)
 # Persist the editable custom geometry before placement; footprint and board match exactly.
 pcb.PCB_IO_MGR.FindPlugin(pcb.PCB_IO_MGR.KICAD_SEXP).FootprintSave(str(LIB),f)
 f.SetPosition(V(*c['xy_mm']));b.Add(f)
for a,z in zip(D['board_outline_mm'],D['board_outline_mm'][1:]+D['board_outline_mm'][:1]):line(b,a,z,pcb.Edge_Cuts,0.05)
def text(s,x,y,size=1,layer=pcb.F_SilkS):
 t=pcb.PCB_TEXT(b);t.SetText(s);t.SetPosition(V(x,y));t.SetTextSize(V(size,size));t.SetTextThickness(MM(0.15));t.SetLayer(layer)
 if layer==pcb.B_SilkS:t.SetMirrored(True)
 b.Add(t)
text('TR2 SHIELD V1.3 DRAFT',55,113.7,1);text('3V3 ONLY / PAS DE 5V',55,116,0.8)
for c in D['components']:
 x,y=c['xy_mm'];text(c['ref'],x+2,y-3 if c['ref']in('C1','C2')else y-2,0.8)
text('SD <',22,92,0.8);text('CAPTEUR / PIN1',51,19,0.8);text('CN12-69 PG4',80,115,0.8);text('J5 FEM / PIN1=69',94,114,0.8)
text('J5 FEM / 1=69 PG4',89,114,0.8,pcb.B_SilkS)
text('STLINK / USB / USER / RESET: KEEP ACCESS',55,14.5,0.8,pcb.Dwgs_User)
# Dimensioned interface extraction consumed by independent router and mechanical review.
result={'outline':D['board_outline_mm'],'pads':[],'footprints':[]}
for f in b.GetFootprints():
 result['footprints'].append({'ref':f.GetReference(),'value':f.GetValue(),'position':[pcb.ToMM(f.GetPosition().x),pcb.ToMM(f.GetPosition().y)],'side':'B'if f.GetLayer()==pcb.B_Cu else'F'})
 for p in f.Pads():result['pads'].append({'ref':f.GetReference(),'number':p.GetNumber(),'net':p.GetNetname(),'x':pcb.ToMM(p.GetPosition().x),'y':pcb.ToMM(p.GetPosition().y),'diameter':pcb.ToMM(p.GetSize().x),'drill':pcb.ToMM(p.GetDrillSize().x),'npth':p.GetAttribute()==pcb.PAD_ATTRIB_NPTH})
pcb.SaveBoard(str(CAD/'TR2_Shield_Proto_V1.kicad_pcb'),b);project_path.write_bytes(project_bytes);(P/'docs/pcb_geometry.json').write_text(json.dumps(result,indent=2)+'\n');print('PCB generated',len(result['pads']),'pads',len(result['footprints']),'footprints')
