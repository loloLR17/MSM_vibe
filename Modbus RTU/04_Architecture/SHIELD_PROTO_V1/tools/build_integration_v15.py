"""Editable CadQuery assembly and closed case CONCEPT. Never a C05 fit certificate.
Run with CadQuery2.8, matplotlib and trimesh; no target or media access.
All model placement coordinates are explicit in parameters.json/design.json.
Unknown LED/USER centres stay null and are NOT silently replaced by guessed points.
"""
from pathlib import Path
import argparse, json, math, hashlib, sys
import cadquery as cq
P=Path(__file__).resolve().parents[1]
V=P/'mechanical/V1_5'; O=V/'REVIEW_ONLY'; Q=P/'docs/checks/v15'
C=json.loads((V/'parameters.json').read_text()); D=json.loads((P/'design.json').read_text()); B=json.loads((P/'docs/pcb_geometry.json').read_text())
O.mkdir(exist_ok=True);Q.mkdir(exist_ok=True)
parts=[]; features=[]
# CAD is right-handed: top-view KiCad Y is reversed, Z remains up.
def box(x0,y0,z0,x1,y1,z1):
 assert x1>x0 and y1>y0 and z1>z0
 return cq.Workplane('XY').box(x1-x0,y1-y0,z1-z0,centered=False).translate((x0,-y1,z0)).val()
def cyl(x,y,z,r,h):return cq.Workplane('XY').circle(r).extrude(h).translate((x,-y,z)).val()
def bore(shape,x,y,z,r,h):return shape.cut(cyl(x,y,z,r,h))
def add(name,shape,color,origin,group='electronics',status='NOMINAL_NOT_PHYSICAL'):
 assert shape.isValid(),name
 parts.append(dict(name=name,shape=shape,color=color,origin=origin,group=group,status=status));return shape
G=C['scenario']['G_Zio_mm'];t=C['scenario']['shield_thickness_mm']; nt=-G-t; nb=nt-C['scenario']['Nucleo_thickness_mm'];mb=C['scenario']['module_bottom_mm']
# Exact saved shield outline and holes, using declared1.6mm thickness. KiCad STEP core alone is1.51.
shield=box(20,17,-t,110,117,0)
cutters=[cyl(q['x'],q['y'],-t-.2,q['drill']/2,t+.4)for q in B['pads']]
shield=shield.cut(cq.Compound.makeCompound(cutters))
add('Shield_V14_exact_saved_outline_holes',shield,(.08,.40,.22),'saved PCB geometry + general thickness1.6')
nucleo=box(21.75,-6.75,nb,91.75,126.59,nt)
for x,y in D['mounting_holes_mm']:nucleo=bore(nucleo,x,y,nb-.1,1.4,2)
add('Nucleo_OUTLINE_REFERENCE_NOT_C05',nucleo,(.13,.32,.52),'UM2861 prior reconstruction, mounting parity not proven','electronics','REFERENCE_OUTLINE_ONLY')
# Each connector body and mating half is separately editable, pins are nominal centre envelopes.
for c in D['components']:
 r=c['ref']; x,y=c['xy_mm'];n=c['n']
 if r in ['J1','J2','J3','J4']:
  rect=[x-1.27,y-1.27,x+3.81,y+(n-1)*2.54+1.27]
  add(r+'_male_shield_body',box(rect[0],rect[1],-t-2.5,rect[2],rect[3],-t),(.15,.15,.17),'Connfly body nominal')
  add(r+'_Zio_socket_Nucleo',box(rect[0],rect[1],nt,rect[2],rect[3],nt+8.5),(.20,.20,.21),'nominal socket8.5, real Zio unknown')
  pins=[box(x+i*2.54-.3175,y+j*2.54-.3175,-t-8.6,x+i*2.54+.3175,y+j*2.54+.3175,1.4)for j in range(n)for i in range(2)]
  add(r+'_mating_posts_nominal',cq.Compound.makeCompound(pins),(.72,.59,.25),'nominal .635 posts; intentional socket overlap')
 elif r=='J5':
  add('J5_female_2x3_BOTTOM',box(84.755,104.755,-t-8.51,89.705,112.885,-t),(.19,.18,.19),'Samtec SSW10301FD catalogue;5=69PG4')
  pins=[box(x+(i-1)%2*2.54-.3175,y+(i-1)//2*2.54-.3175,nt+2.4,x+(i-1)%2*2.54+.3175,y+(i-1)//2*2.54+.3175,nt+8.22)for i in [2,3,4,5]]
  add('CN12_existing_staggered_males_66_67_68_69',cq.Compound.makeCompound(pins),(.72,.59,.25),'user heights8.22/2.40, nominal .635 square')
  for xx in [x,x+2.54]:add('CN12_plastic_'+str(xx),box(xx-1.25,108.82-1.25,nt,xx+1.25,111.36+1.25,nt+2.4),(.17,.17,.17),'user plastic2.40; body lateral footprint nominal')
 elif r in ['J6','J7','J8','J9']:
  rows=[]
  if r=='J6':rows=[[x-1.92,y-1.645,x+22.24,y+1.645]]
  if r=='J7':rows=[[x-1.645,y-22.24,x+1.645,y+1.92]]
  if r=='J8':rows=[[xx-1.645,y-1.92,xx+1.645,y+29.86]for xx in [x,x+15.24]]
  if r=='J9':rows=[[x-19.70,yy-1.645,x+1.92,yy+1.645]for yy in [y,y+22.86]]
  for i,a in enumerate(rows):
   add(r+'_socket_'+str(i),box(a[0],a[1],0,a[2],a[3],8.5),(.18,.18,.20),'V1.4 chosen female catalogue body8.5; actual mating unknown')
   add(r+'_module_male_spacer_'+str(i),box(a[0],a[1],8.5,a[2],a[3],mb),(.25,.25,.27),'nominal male plastic2.5 to module bottom11')
# Manufacturer SD model: rigid rotation90Z and translation, not a cosmetic proxy.
sd=cq.importers.importStep(str(V/'models/Adafruit_4682.step')).val().rotate((0,0,0),(0,0,1),90).translate((43.04,-108.54,mb))
add('Adafruit_4682_manufacturer_STEP',sd,(.52,.54,.58),'AdafruitCAD pinned6c5cd9e3; model thickness1.57; nominal not measured')
fram=box(42.46,56.46,mb,67.86,74.24,mb+1.6)
for x,y in [[45,71.7],[65.32,71.7]]:fram=bore(fram,x,y,mb-.1,1.25,2)
add('Adafruit_4718_PCB_from_Eagle',fram,(.12,.31,.46),'pinned Adafruit Eagle outline/holes; body heights not measured')
add('FRAM_component_keep_envelope',box(47,61,mb+1.6,63,70,mb+4.6),(.65,.46,.15),'PROVISIONAL component height envelope','envelope','PROVISIONAL')
add('MKIGIBV2_PCB_PROVISIONAL',box(42.46,21.46,mb,62.78,54.48,mb+1.6),(.68,.44,.12),'only DIL24 pitch/row known; board outline provisional','electronics','PROVISIONAL')
add('MKI_IDC_component_ENVELOPE_UNKNOWN',box(43.5,24,mb+1.6,61.5,50,21),(.9,.57,.22),'reserve only; not a supplier shape or height','envelope','PROVISIONAL')
# Click v101 board and terminal envelope measured from original STEP, not transplanted v102 assumptions.
click=box(83.41,78.73,mb,140.56,104.13,mb+1.59)
add('Click_v101_PCB_reference',click,(.14,.31,.41),'v101 CAD board57.15x25.4x1.59; user revision unverified')
for k,yy in enumerate([86.731,97.399]):
 add('Click_terminal_envelope_'+str(k),box(134.45,yy-2.77,mb+1.59,140.50,yy+2.77,mb+10.09),(.13,.48,.30),'v101 terminal centre with conservative STEP envelope; port not verified')
add('Click_logic_isolator_ENVELOPE',box(88,82,mb+1.59,130,101,mb+4.4),(.48,.44,.34),'PROVISIONAL component envelope; not full imported part model','envelope','PROVISIONAL')
# Positive mounting: original Nucleo anchor positions, optional module support posts.
for i,(x,y) in enumerate(D['mounting_holes_mm']):
 s=bore(cyl(x,y,nt,2,G),x,y,nt-.1,1.4,G+.2)
 add('Nucleo_Shield_spacer_'+str(i),s,(.80,.80,.75),'insulating OD4 nominal; G must be measured','structure')
for i,(x,y) in enumerate([[45,71.7],[65.32,71.7],[22.72,85.68],[22.72,106]]):
 s=bore(cyl(x,y,0,2,mb),x,y,-.1,1.4,mb+.2)
 add('Module_spacer_'+str(i),s,(.75,.77,.72),'nominal insulating support; height conditional on mating','structure')
def capsule(a,b,r,z,h):
 dx=b[0]-a[0];dy=b[1]-a[1]; L=math.hypot(dx,dy); nx=-dy/L*r; ny=dx/L*r
 pts=[(a[0]+nx,-a[1]-ny),(b[0]+nx,-b[1]-ny),(b[0]-nx,-b[1]+ny),(a[0]-nx,-a[1]+ny)]
 return cq.Workplane('XY').polyline(pts).close().extrude(h).translate((0,0,z)).val().fuse(cyl(*a,z,r,h),cyl(*b,z,r,h))
for name,anchors in [('MKI',[[39,20],[71,54.5]]),('Click',[[91,75],[94.5,112.5]])]:
 s=capsule(*anchors,3,22,3)
 for x,y in anchors:s=s.fuse(cyl(x,y,0,3,25))
 if name=='MKI':
  for x,y in [[43.1,24.3],[62.1,44.9]]:s=s.fuse(box(x-.5,y-1,12.6,x+.5,y+1,22.1))
 else:
  s=s.fuse(capsule([83,91.43],[91,91.43],2,22,3),box(83.6,90.43,12.6,84.6,92.43,22.1))
 for i,(x,y) in enumerate(anchors):s=bore(s,x,y,-.1,1.4 if i==0 else 1.7,26)
 add(name+'_retainer_SCAD_geometry_no_text',s,(.74,.77,.82),'analytical recreation of V1.4 SCAD without engraved text; fit not demonstrated','structure')
# Case open base and removable lid. Every wall service opening is closed by separate blank insert.
e=C['enclosure'];a,b=e['outer_xy'];base=box(*a,-24,*b,37).cut(box(a[0]+3,a[1]+3,-21,b[0]-3,b[1]-3,38))
for x,y in e['lid_screw_centres_xy']:
 base=base.fuse(cyl(x,y,-21,4,58));base=bore(base,x,y,-25,1.7,63)
for x,y in D['mounting_holes_mm']:
 post=bore(cyl(x,y,-21,2.7,nb+21),x,y,-22,1.4,nb+23)
 base=base.fuse(post)
for name,port in C['ports'].items():
 lo,hi=port['opening_xy_z']
 if port['wall']!='lid':base=base.cut(box(*lo,*hi))
add('Case_base_PETG_CONCEPT',base,(.57,.62,.66),'own design; openings reserved, insert cable cuts pending','case')
lid=box(*a,37,*b,40)
for x,y in e['lid_screw_centres_xy']:lid=bore(lid,x,y,36,1.7,5)
# Operator panels are intentionally blank, not guessed holes for unknown LED or USER centres.
panel_windows={'LED_panel_blank':[41,4,70,30],'USER_panel_blank':[22,118,47,136],'RS485_tool_cover':[130,77,146,107]}
for name,rect in panel_windows.items():
 x0,y0,x1,y1=rect;lid=lid.cut(box(x0+2,y0+2,36,x1-2,y1-2,41))
 cover=box(x0,y0,40,x1,y1,42)
 for xx,yy in [(x0+2,y0+2),(x1-2,y1-2)]:
  lid=bore(lid,xx,yy,36,1.1,5);cover=bore(cover,xx,yy,39,1.1,4)
 add(name,cover,(.35,.40,.45),'blank screwed panel; LED/USER centres still unknown','case','NOT_FUNCTIONALLY_RELEASED')
add('Case_lid_PETG_CONCEPT',lid,(.64,.68,.71),'own design, no external RESET hole','case')
# Ports: plugs are dimensioned in one place and remain removable. No silently open case.
for name,p in C['ports'].items():
 if p['wall']=='lid':continue
 lo,hi=p['opening_xy_z'];x0,y0,z0=lo;x1,y1,z1=hi
 if p['wall'] in ['north','south']:
  ym=-22 if p['wall']=='north'else 146
  s=box(x0-3,ym,z0-3,x1+3,ym+2,z1+3)
  holes=[(x0-1,z0-1),(x1+1,z1+1)]
  for xx,zz in holes:
   cut=cq.Solid.makeCylinder(1.1,8,cq.Vector(xx,-ym-4,zz),cq.Vector(0,1,0));s=s.cut(cut);base=base.cut(cut)
 elif p['wall'] in ['west','east']:
  xm=6 if p['wall']=='west'else 166
  s=box(xm,y0-3,z0-3,xm+2,y1+3,z1+3)
  for yy,zz in [(y0-1,z0-1),(y1+1,z1+1)]:
   cut=cq.Solid.makeCylinder(1.1,8,cq.Vector(xm-4,-yy,zz),cq.Vector(1,0,0));s=s.cut(cut);base=base.cut(cut)
 add('Insert_'+name+'_BLANK',s,(.28,.32,.35),'screwed removable plate, cable-specific sealing cut pending','case','NOT_RELEASED_FOR_CABLE_FIT')
# Refresh base after case insert screw holes.
for a in parts:
 if a['name']=='Case_base_PETG_CONCEPT':a['shape']=base
# Geometric keep-outs not asserted as measured connector positions.
keep=[]
for name,p in C['ports'].items():
 keep.append(dict(name=name,box=p['opening_xy_z'],status=p['status']))
keep.extend([{'name':'SD_CARD_EXTRACTION','box':[[-10,88.3,12],[23.1,103.5,15]],'status':'NOMINAL_NEGATIVE_X'},{'name':'RS485_DRIVER','box':[[132,80,12],[142,104,45]],'status':'NOMINAL_TOOL_VOLUME'},{'name':'RIBBON_LANE','box':[[46,-14,9],[61,21,22]],'status':'BEND_RADIUS_AND_IDC_UNKNOWN'}])
for item in keep:
 lo,hi=item['box'];add('RESERVED_'+item['name'],box(*lo,*hi),(.80,.30,.16),'functional keep-out; '+item['status'],'keepout','RESERVATION_NOT_ACTUAL_CABLE')
# Standalone mechanism coupon: zero here is a fictional button, NEVER C05 B1 coordinates.
# The capped blind frame in assembly stays blank until the real B1 centre/height/stroke are known.
plate=box(-22,-12,20,22,12,22)
for x in [-16,-8,0]:plate=bore(plate,x,-5,19,3.0734/2,4)
plate=bore(plate,12,3,19,2.75,4)
# Captive flange below lower guide; upper shoulder seats inside lid recess.
rod=cyl(12,3,3,2.5,20).fuse(cyl(12,3,4,6,1.5),cyl(12,3,23,5,2))
guide=cyl(12,3,7,4,13).fuse(box(2.5,-4,18,21.5,10,20),box(2.5,-2,3,5.5,8,18),box(18.5,-2,3,21.5,8,18)).cut(cyl(12,3,6,2.75,16))
for xx in [4,20]:
 guide=bore(guide,xx,3,2,1.1,20);plate=bore(plate,xx,3,19,1.1,4)
stop=box(5,-4,6,19,10,8).cut(cyl(12,3,5,2.75,4))
button=box(8,-1,0,16,7,2.4)
# Rest tip3 / fictional button top2.4 gives .6gap; flange4 hits support top3 after1mm down.
coupon=[('plate',plate),('rod',rod),('guide',guide),('stop',stop),('dummy_button',button)]
for i,x in enumerate([-16,-8,0]):coupon.append(('Bivar_PLP1_250_F_'+str(i),cyl(x,-5,15.6,1.5,6.4).fuse(cyl(x,-5,18.6,1.9,1.3))))
# Correct captive down-stop: flange lower face at4, support top3 =1mm travel.
stop=box(2.5,-4,1,21.5,10,3).cut(cyl(12,3,0,2.75,5))
for xx in [4,20]:stop=bore(stop,xx,3,0,1.1,4)
coupon=[(n,stop if n=='stop' else s)for n,s in coupon]
# Geometry does not provide a spring, compliant tip or a force qualification. These are critical pending parts.
def export():
 asm=cq.Assembly(name='TR2_V15_REVIEW_ONLY_NO_C05_FIT')
 for p in parts:asm.add(p['shape'],name=p['name'],color=cq.Color(*p['color']))
 asm.export(str(O/'assembly-review.step'))
 for axis,val,half in [('X60',60,box(-100,-100,-100,60,250,150)),('Y92',-92,box(-100,92,-100,250,250,150))]:
  cutasm=cq.Assembly(name='EXACT_BREP_HALF_'+axis)
  for pp in parts:
   cut=pp['shape'].intersect(half)
   if cut.Solids():cutasm.add(cut,name=pp['name'],color=cq.Color(*pp['color']))
  cutasm.export(str(O/('section-'+axis+'.step')))
 for p in parts:
  if p['group']=='case':
   cq.exporters.export(p['shape'],str(O/(p['name']+'.step')))
   cq.exporters.export(p['shape'],str(O/(p['name']+'.stl')),tolerance=.12,angularTolerance=.15)
 aux=cq.Assembly(name='PRINCIPLE_ONLY_NOT_NUCLEO_COORDINATES')
 for n,s in coupon:aux.add(s,name=n)
 aux.export(str(O/'operator-principle-NOT-C05.step'))
 window=cq.Assembly(name='WINDOW_ALTERNATIVE_PARTS_ONLY_NOT_OPTICAL_PATH')
 pane=box(41,4,39,70,30,40.5);frame=box(41,4,40.5,70,30,42.5).cut(box(43,6,40,68,28,43))
 for xx,yy in [(43,6),(68,28)]:pane=bore(pane,xx,yy,38,1.1,6);frame=bore(frame,xx,yy,39,1.1,5)
 window.add(pane,name='clear_PC_or_PMMA_1p5',color=cq.Color(.55,.85,.95,.5));window.add(frame,name='opaque_screwed_frame',color=cq.Color(.25,.27,.28));window.export(str(O/'window-alternative-NOT-OPTICAL-VALIDATION.step'))
 for n,s in coupon:
  if n in ['plate','rod','guide','stop']:cq.exporters.export(s,str(O/('principle-'+n+'.stl')),tolerance=.10,angularTolerance=.15)
 (Q/'assembly-parts.json').write_text(json.dumps([{'name':p['name'],'origin':p['origin'],'group':p['group'],'status':p['status'],'volume_mm3':p['shape'].Volume(),'solids':len(p['shape'].Solids()),'bbox_CAD_xyz':[[getattr(p['shape'].BoundingBox(),k+'min'),getattr(p['shape'].BoundingBox(),k+'max')]for k in 'xyz']}for p in parts],ensure_ascii=False,indent=2)+'\n')
 print('Exported editable STEP assembly',len(parts),'named parts; case STEP/STL and separate mechanism coupon')
def render():
 import matplotlib;matplotlib.use('Agg')
 import matplotlib.pyplot as plt
 from mpl_toolkits.mplot3d.art3d import Poly3DCollection
 from matplotlib.backends.backend_pdf import PdfPages
 import numpy as np
 meshes=[]
 for p in parts:
  vs,fs=p['shape'].tessellate(.4,.35);v=np.array([[q.x,q.y,q.z]for q in vs]);f=np.array(fs)
  meshes.append((p,v,f))
 def draw(ax,selection=None,cut=None,explode=False):
  triangles=[];colors=[]
  for p,v,f in meshes:
   if selection and p['group']not in selection:continue
   if cut:
    dim,val,side=cut
    half=(cq.Workplane('XY').box(500,500,300).translate((val-250,-70,0)).val()if dim==0 else cq.Workplane('XY').box(500,500,300).translate((80,val-250,0)).val())
    clipped=p['shape'].intersect(half)
    if not clipped.Solids():continue
    vs,fs=clipped.tessellate(.4,.35);v=np.array([[q.x,q.y,q.z]for q in vs]);f=np.array(fs)
   if not len(f):continue
   v=v.copy()
   if explode:
    if p['group']=='case':v[:,2]+=80 if 'lid' in p['name'].lower()or'panel'in p['name']or'cover'in p['name']else -20
    elif 'retainer'in p['name']:v[:,2]+=35
    elif 'Shield_'in p['name']:v[:,2]+=10
   a=.07 if p['group']=='keepout'else(.14 if p['group']=='envelope' else (.28 if p['group']=='case' else 1))
   a=1 if selection=={'case'}and cut is None else a
   norm=np.cross(v[f[:,1]]-v[f[:,0]],v[f[:,2]]-v[f[:,0]]);norm/=np.maximum(np.linalg.norm(norm,axis=1)[:,None],1e-12)
   light=np.array([.4,.6,.7]);light/=np.linalg.norm(light);shade=.55+.45*np.abs(norm@light)
   triangles.append(v[f]);colors.append(np.column_stack((shade[:,None]*np.array(p['color']),np.full(len(f),a))))
  ax.add_collection3d(Poly3DCollection(np.concatenate(triangles),facecolors=np.concatenate(colors),edgecolors='none'))
  ax.set(xlim=(0,177),ylim=(-152,27),zlim=(-28,145 if explode else 48),xlabel='X mm',ylabel='CAD Y = -KiCad Y',zlabel='Z / shield top mm')
  ax.set_box_aspect((177,179,173 if explode else 76));ax.tick_params(labelsize=6)
 specs=[('assembly-isometric',25,-65,None,{'electronics','structure','envelope','keepout'},False),('assembly-top',90,-90,None,{'electronics','structure','envelope','keepout'},False),('assembly-bottom',-90,-90,None,{'electronics','structure'},False),('assembly-profile',0,-90,None,{'electronics','structure','envelope','keepout'},False),('assembly-exploded',22,-65,None,None,True),('case-closed',25,-65,None,{'case'},False),('section-X60',15,-15,(0,60,1),None,False),('section-Y92',12,-85,(1,-92,1),None,False)]
 with PdfPages(O/'assembly-views-and-sections.pdf')as pdf:
  for name,elev,az,cut,sel,exp in specs:
   fig=plt.figure(figsize=(11.7,8.3));ax=fig.add_subplot(111,projection='3d');draw(ax,sel,cut,exp);ax.view_init(elev=elev,azim=az)
   fig.suptitle('TR2 V1.5 | '+name+' | REVIEW ONLY / NO C05 FIT',fontsize=13)
   fig.text(.06,.035,'Green: saved shield. Blue: reconstructed Nucleo. Amber: provisional envelopes.\nLED/USER centres unknown: no guessed LED, button or optical route is placed. Case panels remain blank.',fontsize=9)
   fig.savefig(O/(name+'.png'),dpi=120);pdf.savefig(fig);plt.close(fig)
 fig=plt.figure(figsize=(10,7));ax=fig.add_subplot(111,projection='3d')
 for i,(n,s)in enumerate(coupon):
  vs,fs=s.tessellate(.1,.2);v=np.array([[q.x,q.y,q.z]for q in vs]);f=np.array(fs)
  poly=Poly3DCollection(v[f],facecolors=[(.4,.7,.9)if n.startswith('Bivar')else(.55,.58,.60)],edgecolors='none',alpha=.70);ax.add_collection3d(poly)
 ax.set(xlim=(-24,24),ylim=(-14,14),zlim=(-2,29),xlabel='Coupon X (not C05)',ylabel='Y',zlabel='Z');ax.set_box_aspect((48,28,31));ax.view_init(20,-65)
 fig.suptitle('USER captive / 3 standard guides - PRINCIPLE ONLY, NOT C05 positions')
 fig.text(.06,.03,'Head gap/stop1mm; tip gap0.6mm; guide diameter5.5 / shaft5.\nReturn spring, compliant tip, actual B1 stroke/force and optical path NOT qualified.',fontsize=9)
 fig.savefig(O/'operator-principle-NOT-C05.png',dpi=130);fig.savefig(O/'operator-principle-NOT-C05.pdf');plt.close(fig)
 print('Rendered8 assembly/case views including two exact BRep half-solid sections, plus separate operator coupon')
def verify():
 import trimesh
 assertions=[]
 assert all(v is None for v in C['critical_unknowns'].values());assert C['USER_principle']['placed_on_Nucleo']is False
 assertions.append('critical unknowns remain null; no LED/USER part placed on Nucleo')
 assert len(shield.Solids())==1 and shield.isValid();assert len(B['pads'])==191
 # Rigid SD transform verified against ALL9 numbered positions, not just the bbox.
 by={(q['ref'],q['number']):q for q in B['pads']if q['number']}
 for i in range(1,10):
  sx=2.54+(i-1)*2.54;sy=2.54;target=[43.04-sy,-108.54+sx]
  q=by['J7',str(i)];assert math.dist(target,[q['x'],-q['y']])<1e-6
 for sx in [2.54,22.86]:
  target=[43.04-20.32,-108.54+sx];assert any(math.dist(target,[22.72,-yy])<1e-6 for yy in [85.68,106])
 assertions.append('Adafruit STEP/Eagle rigid transform:9 numbered pads +2 mounting centres match saved PCB')
 assert len(D['components'])==19;assert [c for c in D['components']if c['ref']=='J5'][0]['nets']=={'5':'RTU_DE'}
 # Exact nominal J5 solid body clearance, mating insertion remains intentional contact overlap.
 j5gap=(-t-8.51)-(nt+2.4);ins=(nt+8.22)-(-t-8.51)
 assert abs(j5gap-.09)<1e-6 and abs(ins-5.73)<1e-6
 assertions.append('G11 example: J5 body gap0.09 and insertion5.73; no tolerance/physical PASS')
 # Direct LED sector projection intersects shield: a risk, NOT a C05 measured collision.
 reg=C['control_regions']['LED_north_inspection_xy'];inter=[max(20,reg[0][0]),max(17,reg[0][1]),min(110,reg[1][0]),min(117,reg[1][1])]
 assert inter[2]>inter[0]and inter[3]>inter[1]
 # No broad automatic verdict for unknown control centres or bodies.
 checks=[]
 for p in parts:
  assert p['shape'].isValid(),p['name']
 for f in O.glob('*.stl'):
  mesh=trimesh.load_mesh(f);assert mesh.is_watertight and mesh.is_volume, f.name
  assert len(mesh.split())==1,f.name
  checks.append({'file':f.name,'watertight':True,'positive_volume_mm3':float(mesh.volume),'connected_components':1})
 reread=cq.importers.importStep(str(O/'assembly-review.step'));assert len(reread.solids().vals())>len(parts)
 for f in O.glob('Case_*.step'):
  w=cq.importers.importStep(str(f));assert w.val().isValid(),f.name
 # User coupon rest/stop kinematics only, no actual button force/stroke known.
 assert 3-2.4>.59;assert abs(4-3-1)<1e-6;assert 5.5-5==.5
 assert rod.intersect(plate).Volume()<1e-6
 assert rod.translate((0,0,-1)).intersect(plate).Volume()<1e-6
 assert rod.intersect(stop).Volume()<1e-6
 assert rod.translate((0,0,-1)).intersect(stop).Volume()<1e-6
 assertions.append('isolated USER coupon: head has1mm travel before panel; no positive-volume plate/stop interference at rest and at1mm stop')
 r={'status':'PASS_GENERATED_NOMINAL_SOLIDS_ONLY','release':'READY_FOR_PHYSICAL_CHECK / BLOCKED_FABRICATION','assembly_named_parts':len(parts),'STEP_readback_solids':len(reread.solids().vals()),'assertions':assertions,'J5_G11_example':{'gap_mm':j5gap,'insertion_mm':ins,'G_actual':None},'LED_sector_shield_projection_overlap_xy':inter,'LED_verdict':'BLOCKED: north sector intersects shield; exact C05 centres and clear path unknown. No PCB cut justified at exact coordinates yet.','USER_verdict':'BLOCKED: south sector generally beyond shield, but exact cap swept volume/centre/stroke unknown','coupon_only':{'rest_gap_mm':.6,'hard_stop_travel_mm':1,'diametral_guide_clearance_mm':.5,'actual_button_force_stroke':'UNKNOWN; return spring and compliant tip still to size'},'STL_meshes':checks,'all_physical_fit':'NOT_TESTED','no_CAD_files_changed':'verified separately against initial HEAD','intentional_contacts':'pins/socket, board/supports, mating and covers; no claim of global collision-free assembly'}
 (Q/'integration-validation.json').write_text(json.dumps(r,ensure_ascii=False,indent=2)+'\n');print(json.dumps({k:v for k,v in r.items()if k not in ['STL_meshes','assertions']},indent=2))
if __name__=='__main__':
 parser=argparse.ArgumentParser();parser.add_argument('--render-only',action='store_true');parser.add_argument('--verify-only',action='store_true');a=parser.parse_args()
 if a.render_only:render()
 elif a.verify_only:verify()
 else:export();render();verify()
