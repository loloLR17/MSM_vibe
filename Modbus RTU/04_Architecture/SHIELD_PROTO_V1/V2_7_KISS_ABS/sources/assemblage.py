"""One coherent prototype, supplier-derived envelopes + own parts, editable exploded STEP.

No exact ST/Mikroe/Adafruit solid is redistributed. Native supplier boxes are
factual design envelopes explicitly named and sourced. No spare/alternative in assembly.
"""
from pathlib import Path
import json,math
import cadquery as cq
import modele as v
import controle as c
P=v.P;m=v.m
COL={'case':(.68,.73,.79),'PCB':(.16,.46,.30),'header':(.16,.18,.21),'metal':(.57,.61,.67),'nylon':(.91,.89,.78),'MKI':(.88,.49,.17),'Click':(.22,.51,.76),'support':(.76,.41,.16),'USER':(.55,.29,.65),'wire':(.17,.40,.64),'soft':(.15,.17,.18)}
def compound(ss):return cq.Compound.makeCompound(ss)
def worldbox(b):return cq.Solid.makeBox(b[1]-b[0],b[3]-b[2],b[5]-b[4],cq.Vector(b[0],b[2],b[4]))
def module_boxes(name,H=11.2):
 B=13.85+H
 if name=='Click':
  data=json.loads((c.D/'Click2-solids-raw.json').read_text())['solids'];ss=[worldbox(q['bbox']).rotate((0,0,0),(0,0,1),-90).translate((87.22,-80,B+1.6))for q in data]
 else:
  data=json.loads((c.D/'SD-solids-raw.json').read_text());ss=[]
  for q in data:
   s=worldbox(q['bbox']).rotate((0,0,0),(0,0,1),90).translate((43.04,-108.54,B))
   if q['i']==0:
    for x,y in m.C['module_supports_SD']:s=s.cut(m.cyl(x,y,B-.1,1.25,1.9))
   ss.append(s)
 return compound(ss)
def nucleo():
 meta=json.loads((P.parent/'V2_0_EMBOITEMENT/preuves/C05-collisions-G12p25.json').read_text());tx,ty=meta['Tx_Ty'];by={q['index']:q['bbox_ST']for q in meta['source_geometry']};items=[];prov=[]
 board=cq.Workplane('XY').polyline([(x,-y)for x,y in m.D['Nucleo_outline']]).close().extrude(1.6).val().translate((0,0,-1.6))
 for x,y in m.C['anchors']:board=board.cut(m.cyl(x,y,-1.7,1.35,1.8))
 items.append(('PCB_C05_outline_ST',board,'PCB'))
 for n,ids in [('Zio',[892,895,898,901]),('CN1',[95,96,97]),('CN15',[451,471]),('B1_source',[502,515,516,517])]:
  ss=[]
  for i in ids:
   b=by[i];ss.append(worldbox(b).translate((tx,-ty,0)));prov.append({'name':n,'source_index':i,'bbox_ST':b})
  items.append((n+'_enveloppes_ST',compound(ss),'header'))
 # B1 actual reported top10.2, not equated to simplified STEP source7.55.
 items.append(('B1_top_reported_not_force_model',m.box(v.UX-1.2,v.UY-1.2,7.55,v.UX+1.2,v.UY+1.2,10.2),'Click'))
 rx,ry=m.C['RESET']['xy'];items.append(('RESET_axis_envelope',m.box(rx-3,ry-3,0,rx+3,ry+3,7.55),'header'))
 for q in m.C['LEDs']:
  x,y=q['xy_shield_mm'];items.append((q['ref']+'_position_ST',m.box(x-.6,y-.5,0,x+.6,y+.5,q['height_mm']),'MKI'if q['ref']=='LD3'else'Click'if q['ref']=='LD2'else'PCB'))
 return items,prov

def groups():
 if not v.PARTS:v.build()
 groups={}
 labels={1:'Caisse et écrous captifs',2:'Façades fixes et vitre',3:'Nucleo C05 (enveloppes ST)',4:'D126, bras et Ajile',5:'Shield V2.3 et connecteurs',6:'Selles choisies, M2 et rondelles',7:'FRAM et microSD',8:'MKI et Click',9:'Étriers et patins isolants',10:'Brides et couloirs câbles',11:'Cage USER, ressort, butée',12:'Capot ARNACO et USER captif'}
 offsets={1:(0,0,0),2:(0,0,0),3:(0,0,52),4:(0,0,78),5:(0,0,102),6:(0,0,124),7:(0,0,162),8:(0,0,188),9:(0,0,220),10:(0,0,258),11:(0,0,300),12:(0,0,342)}
 for k in labels:groups[k]={'id':k,'label':labels[k],'offset':offsets[k],'items':[]}
 def add(g,n,s,color,alpha=1):groups[g]['items'].append({'name':n,'shape':s,'color':COL[color],'alpha':alpha})
 for row in v.INST:
  n=row['id'];g=1 if n=='01_base_ABS'else 12 if n.startswith(('02_','41_'))else 2 if n.startswith(('03_','06_','07_'))else 4 if n in ['13_D126','15_retenue_Nucleo']else 6 if n.startswith('20_')else 9 if n.startswith(('30_','31_','32_','33_','34_'))else 10 if n.startswith(('10_','11_'))else 11
  co='USER'if g in [11,12]and n.startswith(('40_','41_'))else'MKI'if'MKI'in n else'Click'if'Click'in n else'support'if g==6 else'nylon'if n=='13_D126'else'case'
  add(g,row['label'],row['shape'],co,.3 if g==1 else 1)
 ni,prov=nucleo()
 for n,s,col in ni:add(3,n,s,col)
 ref=dict(c.refs())
 add(5,'PCB_V23_holes_and_RC_envelopes',ref['shield_actual_holes_RC_envelopes'],'PCB')
 add(5,'FRAM_1x10_intact',ref['FRAM_socket'],'header');add(5,'SD_1x10_intact',ref['SD_socket'],'header')
 meta=json.loads((P.parent/'V2_0_EMBOITEMENT/preuves/C05-collisions-G12p25.json').read_text());tx,ty=meta['Tx_Ty'];by={q['index']:q['bbox_ST']for q in meta['source_geometry']}
 for i in [892,895,898,901]:
  b=by[i];add(5,'Zio_male_body_'+str(i),m.box(b[0]+tx,ty-b[3],9.75,b[1]+tx,ty-b[2],12.25),'header')
 p5=[q for q in m.D['pads']if q['ref']=='J5'];xs=[q['x']for q in p5];ys=[q['y']for q in p5]
 add(5,'J5_SSW103_envelope',m.box(min(xs)-1.3,min(ys)-1.3,3.74,max(xs)+1.3,max(ys)+1.3,12.25),'header')
 for x in [45,60.24]:add(5,'MKI_socket_'+str(x),m.box(x-1.25,22.08,13.85,x+1.25,53.86,22.35),'header')
 for y in [80,102.86]:add(5,'Click_socket_'+str(y),m.box(85.3,y-1.25,13.85,106.92,y+1.25,22.35),'header')
 add(7,'FRAM_Eagle_envelopes',ref['FRAM_envelope'],'MKI');add(7,'SD_6_supplier_derived_boxes',module_boxes('SD'),'Click')
 add(8,'MKI_Gerber_derived_envelopes',ref['MKI_Gerber_component_envelopes'],'MKI');add(8,'Click_v101_163_supplier_derived_boxes_NOT_v102',module_boxes('Click'),'Click')
 for j,(x,y)in enumerate(m.C['anchors']):
  add(3,'Ajile_M2x20_envelope_'+str(j),c.bolt(x,y,-1.6,20,1,3.2,3.3),'nylon')
  for k in [0,1]:add(5,f'Ajile_nut{j}_{k}',c.nut(x,y,13.85+1.6*k,4,1.6),'nylon')
 for kind in ['FRAM','SD']:
  for j,(x,y)in enumerate(m.C['module_supports_'+kind]):
   top=13.85+11.2+(1.6 if kind=='FRAM'else 1.57)
   add(5,f'{kind}{j}_M2x30',c.bolt(x,y,11.95),'metal')
   for g,z,tag in [(5,11.95,'lower'),(7,top,'upper')]:add(g,f'{kind}{j}_{tag}_washer_DIN433',m.cyl(x,y,z,2.25,.3).cut(m.cyl(x,y,z-.1,1.1,.5)),'nylon')
   for k in [0,1]:add(7,f'{kind}{j}_nut{k}',c.nut(x,y,top+.3+1.6*k,4,1.6),'metal')
 for which in ['MKI','Click']:
  for j,(x,y)in enumerate(v.v.CONFIG[which]['anchors']):
   add(5,f'{which}{j}_M2x30',c.bolt(x,y,11.95),'metal');add(5,f'{which}{j}_lower_washer',m.cyl(x,y,11.95,2.25,.3).cut(m.cyl(x,y,11.9,1.1,.5)),'nylon')
   for k in [0,1]:add(9,f'{which}{j}_nut{k}',c.nut(x,y,34.5+1.6*k,4,1.6),'metal')
  for i,(x,y)in enumerate(v.v.CONFIG[which]['guides']):
   add(9,f'{which}{i}_M3_adjustment',c.bolt(x,y,26.65+13.5,12,1.5,3,3.2,False),'metal');add(9,f'{which}{i}_DIN985',c.nut(x,y,32.8,5.5,4,1.5),'metal')
 for j,(kn,x,y,z,u,a)in enumerate(c.case_mounts()):
  g=12 if kn in['lid','USERcage']else 4 if kn=='keeper'else 2 if kn=='window'else 10;p=(x,-y,z)
  b=cq.Solid.makeCylinder(1.5,12,cq.Vector(*p),cq.Vector(*u)).fuse(cq.Solid.makeCylinder(3,3.2,cq.Vector(*p),cq.Vector(*[-t for t in u])))
  n=c.nut(0,0,0,5.5,2.4,1.5).rotate((0,0,0),(1,0,0),180 if u==(0,0,-1)else 90).translate(tuple(p[k]+a*u[k]for k in range(3)))
  add(g,f'M3case{j}_{kn}',b,'metal');add(1 if kn in ['lid','keeper','RSbar']else 11 if kn=='USERcage'else g,f'M3nut{j}_{kn}',n,'metal')
 x,y=v.STOP;add(11,'M3_stop_SHIPPING_SHORT_0p2',c.bolt(x,y,32.8,12,1.5,3,3.2),'metal');add(11,'DIN985_stop',c.nut(x,y,34.6,5.5,4,1.5),'metal')
 spring=m.cyl(v.UX,v.UY,36,3.525,9).cut(m.cyl(v.UX,v.UY,35.9,2.75,9.2));add(11,'D064_spring_annular_envelope_NOT_coils',spring,'metal',.5)
 add(2,'PC_window_50x18x1p5',m.box(32.5,-15.8,11,82.5,-14.3,29),'wire',.2)
 add(10,'RS485_cable_D6_design_not_measured',cq.Solid.makeCylinder(3,28,cq.Vector(144,-92.5,34.5),cq.Vector(1,0,0)),'wire')
 add(10,'RS485_soft_protection',m.box(151,89,30,157,96,31.5),'soft')
 # Ribbon envelope sampled along the same tested route; not a new cable MPN.
 import numpy as np
 pts=np.array([[52.635,-33.385,35.15],[52.635,-17,37.15],[55.5,0,43],[55.5,22,43]])
 cv=[(1-t)**3*pts[0]+3*t*(1-t)**2*pts[1]+3*t*t*(1-t)*pts[2]+t**3*pts[3]for t in np.linspace(0,1,21)];ss=[]
 for a,b in zip(cv,cv[1:]):
  lo=np.minimum(a,b);hi=np.maximum(a,b);ss.append(m.box(lo[0]-4,-hi[1],lo[2]-.5,hi[0]+4,-lo[1],hi[2]+.5))
 add(10,'ribbon_8x1_design_envelope',compound(ss),'wire')
 # Rectangular tie envelopes through designed slots; exact commercial ties not modeled.
 tie=m.box(153.1,87.1,26.4,154.9,97.9,38.5).cut(m.box(153,88.3,27.3,155,96.7,37.7));add(10,'Colson_RS485_envelope',tie,'soft')
 tie=m.box(46.4,-14,36.4,60.6,-12.4,44).cut(m.box(47.6,-14.1,37.2,59.4,-12.3,43.2));add(10,'Colson_ribbon_envelope',tie,'soft')
 return groups,prov

def explode_offset(g,row):
 if g['id']==2:
  return(30,0,12)if row['name'].startswith('06_')else(0,30,12)
 return g['offset']

def main():
 gg,prov=groups();a=cq.Assembly(name='V27_ASSEMBLED_OWN_AND_DERIVED_ENVELOPES_NOT_PHYSICAL');e=cq.Assembly(name='V27_EXPLODED_ONE_PROTOTYPE_NO_VARIANTS')
 meta=[]
 for i,g in gg.items():
  for j,row in enumerate(g['items']):
   name=f'{i:02d}_{j:02d}_{row["name"]}';a.add(row['shape'],name=name,color=cq.Color(*row['color'],row['alpha']));e.add(row['shape'].translate(explode_offset(g,row)),name=name,color=cq.Color(*row['color'],row['alpha']))
  meta.append({'repere':i,'label':g['label'],'explosion_translation_CAD_mm':g['offset'],'objects':[{'name':r['name'],'translation':explode_offset(g,r)}for r in g['items']]})
 for asm,n in [(a,'assemblage-complet-enveloppes.step'),(e,'eclate-un-prototype.step')]:asm.save(str(P/'cad'/n));v.trim(P/'cad'/n)
 (P/'preuves/reperes-eclate.json').write_text(json.dumps(meta,indent=2,ensure_ascii=False)+'\n');(P/'sources/C05-enveloppes-derivees.json').write_text(json.dumps({'source':'V2_0_EMBOITEMENT/preuves/C05-collisions-G12p25.json','source_ST_sha256':'55f465f8de566561905f97e4b10533a4de33dd13842a5bd9f25bc9d7f06a9900','objects':prov,'B1_top10p2':'user observation; sourcepattern HEIGHT7.55 not treated as complete actual cap','vendor_solids_redistributed':False},indent=2,ensure_ascii=False)+'\n')
 print(json.dumps({'groups':len(gg),'objects':sum(len(g['items'])for g in gg.values()),'single_prototype':True,'variants_or_spares_in_assembly':False,'STEP':'eclate-un-prototype.step','supplier_objects':'own derived bounding envelopes, no vendor solids'},indent=2),flush=True)
if __name__=='__main__':main()
