"""Minimal candidate correction from frozen V2.3, never writes historical sources.

Only J8 electrical metadata, its six affected contacts, schematic labels/NC and
four SPI3 signal route groups change. All physical component geometry stays fixed.
"""
from pathlib import Path
import copy,json,uuid
import sexpdata as sx
P=Path(__file__).resolve().parents[1]
BASE=P.parent/'V2_3_ARNACO'
S=sx.Symbol

def tag(n,t):return isinstance(n,list) and bool(n) and str(n[0])==t
def one(n,t):return next(x for x in n if tag(x,t))
def dump(n):return sx.dumps(n)+'\n'
new={19:('ACC_CS','CS','input'),20:('ACC_SCK','SCL','input'),21:('ACC_MOSI','SDA','input'),22:('ACC_MISO','SDO','output'),23:('NC','NC','passive'),24:('NC','NC','passive')}

def pins(node):
 if not isinstance(node,list):return
 if tag(node,'pin') and isinstance(node[1],S):
  number=int(one(node,'number')[1])
  if number in new:
   node[1]=S(new[number][2]);one(node,'name')[1]=new[number][1]
 else:
  for child in node:pins(child)

# Schematic and external library share the same corrected pin names and types.
sch=sx.loads((BASE/'cad/TR2_Shield_Proto_V1.kicad_sch').read_text())
for symbol in one(sch,'lib_symbols')[1:]:
 if tag(symbol,'symbol') and symbol[1]=='TR2:J8_ST_MKIGIBV2':pins(symbol)
for n in sch:
 if tag(n,'wire'):
  pts=one(n,'pts')[1:]
  if float(pts[0][1])==294.64 and 52.06<float(pts[0][2])<59.70:
   for pt in pts:pt[2]=round(float(pt[2])+5.08,2)
 elif tag(n,'global_label'):
  at=one(n,'at')
  if float(at[1])==300.99 and 52.06<float(at[2])<59.70:at[2]=round(float(at[2])+5.08,2)
 elif tag(n,'no_connect'):
  at=one(n,'at')
  if float(at[1])==294.64 and float(at[2])in(62.23,64.77):at[2]=52.07 if float(at[2])==62.23 else 54.61
(P/'cad/TR2_Shield_Proto_V1.kicad_sch').write_text(dump(sch))
lib=sx.loads((BASE/'cad/TR2.kicad_sym').read_text())
for symbol in lib:
 if tag(symbol,'symbol') and symbol[1]=='J8_ST_MKIGIBV2':pins(symbol)
(P/'cad/TR2.kicad_sym').write_text(dump(lib))

board=sx.loads((BASE/'cad/TR2_Shield_Proto_V1.kicad_pcb').read_text())
for fp in board:
 if tag(fp,'footprint') and any(tag(q,'property')and q[1]=='Reference'and q[2]=='J8'for q in fp):
  for pad in fp:
   if tag(pad,'pad') and pad[1] and int(pad[1])in new:
    number=int(pad[1]);net,func,typ=new[number]
    if net=='NC':net=f'unconnected-(J8-NC-Pad{number})'
    one(pad,'net')[1:]=[net]  # KiCad10 native board format: name, no numeric net code

# Remove only the five affected terminal route branches. Preserve R1->J2 CS,
# the SCK via->J2 prefix, all four vias and every other net's existing copper.
routes=json.loads((BASE/'cad/routes.json').read_text())
affected=[];keep=[]
for route in routes['routes']:
 if route['net'].startswith('ACC_') and (route['from'].startswith('J8.')or route['to'].startswith('J8.')):
  affected.append(route)
 else:keep.append(route)
remove=set()
for route in affected:
 for a,b in zip(route['points'],route['points'][1:]):
  if a['layer']!=b['layer']:continue
  if route['net']=='ACC_SCK' and route['from']=='J8.22' and a['layer']==0:continue
  pa=tuple(round(x,5)for x in a['xy']);pb=tuple(round(x,5)for x in b['xy'])
  remove.add((route['net'],a['layer'],tuple(sorted((pa,pb)))))
removed=[];out=[]
for node in board:
 if tag(node,'segment'):
  start=tuple(round(float(x),5)for x in one(node,'start')[1:]);end=tuple(round(float(x),5)for x in one(node,'end')[1:]);layer=0 if one(node,'layer')[1]=='F.Cu'else 1;net=one(node,'net')[1]
  if (net,layer,tuple(sorted((start,end))))in remove:removed.append(node);continue
 out.append(node)
assert len(removed)==len(remove),(len(removed),len(remove))
(P/'cad/TR2_Shield_Proto_V1.kicad_pcb').write_text(dump(out))
D=json.loads((BASE/'design.json').read_text());D['version']='2.4-SPI3-candidate';D['mission']='TR2-20261118-SHIELD-V24-SPI3-GERBER-CORRECTIF';D['release']='CANDIDAT_NON_LIBERE'
for c in D['components']:
 if c['ref']=='J8':
  for k in ('19','20','21','22','23','24'):c['nets'].pop(k,None);c['pin_types'].pop(k,None)
  for pin,(net,func,typ)in new.items():
   if net!='NC':c['nets'][str(pin)]=net;c['pin_types'][str(pin)]=typ
(P/'design.json').write_text(json.dumps(D,indent=2,ensure_ascii=False)+'\n')
(P/'preuves/correction-initiale.json').write_text(json.dumps({'removed_signal_segments':len(removed),'new_J8':new,'unchanged_vias':4,'route_status':'terminal branches removed; rerouting pending; no export allowed'},indent=2)+'\n')
print('Corrected J8 schematic/library/pads; removed only',len(removed),'terminal signal segments; all vias retained')
