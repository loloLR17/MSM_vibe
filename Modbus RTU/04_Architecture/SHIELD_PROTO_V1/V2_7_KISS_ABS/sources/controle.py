"""Targeted V27 geometry, USER custody/stop, source envelopes and cable assembly checks."""
from pathlib import Path
import json,math,itertools,hashlib
import cadquery as cq
import numpy as np
import trimesh
import modele as v
P=v.P;m=v.m;D=Path('/home/lolo/dev/msm/reports/TR2-shield-v26-20261105')
def save(n,x):(P/'preuves'/n).write_text(json.dumps(x,indent=2,ensure_ascii=False)+'\n')
def bb(s):
 b=s.BoundingBox();return[b.xmin,b.xmax,b.ymin,b.ymax,b.zmin,b.zmax]
def near(a,b):return all(min(a[i+1],b[i+1])-max(a[i],b[i])>1e-7 for i in [0,2,4])
def volume(a,b):return abs(a.intersect(b).Volume())if near(bb(a),bb(b))else 0
CHECKS=[];HITS=[]
def clear(n,a,b):
 vol=volume(a,b);q={'check':n,'positive_volume_mm3':vol};CHECKS.append(q)
 if vol>1e-5:HITS.append(q)
def nut(x,y,z,af,h,bore=1):return m.hexhole(x,y,z,af,h).cut(m.cyl(x,y,z-.1,bore,h+.2))
def bolt(x,y,z,L=30,r=1,hr=2.1,hh=2.2,up=True):
 if up:return m.cyl(x,y,z,r,L).fuse(m.cyl(x,y,z-hh,hr,hh))
 return m.cyl(x,y,z-L,r,L).fuse(m.cyl(x,y,z,hr,hh))
def passive_envelopes():
 rows=[]
 for ref in ['R1','R2','R3','R4','C1','C2']:
  pads=[q for q in m.D['pads']if q['ref']==ref];x=sum(q['x']for q in pads)/len(pads);y=sum(q['y']for q in pads)/len(pads)
  if ref.startswith('R'):w,d,h=4.5,2.5,3.5 # MRS16 source3.6x1.6, extra mounting allowance
  elif ref=='C1':w,d,h=6,4,7 # SR205 source5.08x3.175x5.08, conservative standoff
  else:w,d,h=6,6,14 # EEUFR1H100 sourceD5H11, plus mounting allowance
  rows.append((ref,m.box(x-w/2,y-d/2,13.85,x+w/2,y+d/2,13.85+h)))
 return rows

def refs(H=11.2):
 B=13.85+H;rows=[]
 sd=cq.Shape.importBrep(str(D/'SD-cache.brep')).rotate((0,0,0),(0,0,1),90).translate((43.04,-108.54,B));rows.append(('SD_native',sd))
 click=cq.Shape.importBrep(str(D/'Click2-cache.brep')).rotate((0,0,0),(0,0,1),-90).translate((87.22,-80,B+1.6));rows.append(('Click_native',click))
 fram=m.box(42.46,56.46,B,67.86,74.24,B+1.6)
 for x,y in m.C['module_supports_FRAM']:fram=fram.cut(m.cyl(x,y,B-.1,1.25,1.8))
 for q in json.loads((v.V26/'sources/adafruit-derives.json').read_text())['FRAM']['component_envelopes']:
  x,y,X,Y=q['bbox_XY'];fram=fram.fuse(m.box(x,y,B+1.6,X,Y,B+5.6))
 rows.append(('FRAM_envelope',fram))
 mk=m.box(43.612906,22.511306,B,61.611854,53.438346,B+1.6)
 for x in [45,60.239746]:
  for j in range(12):mk=mk.cut(m.cyl(x,24+2.54*j,B-.1,.45,1.8))
 mk=mk.fuse(m.box(46.5,25.8,B+1.6,58.8,41,B+10.1))
 for x in [45,60.239746]:mk=mk.fuse(m.box(x-1.3,22.7,B-2.5,x+1.3,53.3,B+1.6))
 for x,y in [(55.884,46.83),(54.795,46.83),(49.205,46.81),(50.295,46.81)]:mk=mk.fuse(m.box(x-.65,y-.8,B+1.6,x+.65,y+.8,B+3.6))
 rows.append(('MKI_Gerber_component_envelopes',mk))
 # Actual pad centres; manufacturer body dimensions plus explicit mounting allowances.
 sh=m.box(20,17,12.25,110,117,13.85)
 for p in m.D['pads']:sh=sh.cut(m.cyl(p['x'],p['y'],12.2,p['drill']/2,1.8))
 for name,component in passive_envelopes():sh=sh.fuse(component)
 rows.append(('shield_actual_holes_RC_envelopes',sh))
 sockets=[('FRAM_socket',m.box(43.08,57.75,13.85,69.78,60.25,22.35)),('SD_socket',m.box(39.25,81.22,13.85,41.75,107.92,22.35))]
 rows+=sockets;return rows

def case_mounts():
 mounts=[]
 for x,y in m.C['lid_screw_centres']:mounts.append(('lid',x,y,50,(0,0,-1),5.4))
 for x,y in m.C['case_keeper_pivots']:mounts.append(('keeper',x,y,4.4,(0,0,-1),5.4))
 for y in [82,101]:mounts.append(('RSbar',154,y,30,(0,0,-1),7.4))
 for x in [30.5,85]:mounts.append(('window',x,-18.2,20,(0,-1,0),5.9))
 for x in [42.5,64.5]:mounts.append(('ribbon',x,-18.2,38,(0,-1,0),7.7))
 for x,y in v.BOSSES:mounts.append(('USERcage',x,y,50,(0,0,-1),7.0))
 assert len(mounts)==15
 return mounts

def main():
 v.build();rows=json.loads((P/'preuves/pieces.json').read_text());mesh=[]
 for r in rows:
  f=P/r['STL'];g=P/r['STEP'];assert hashlib.sha256(f.read_bytes()).hexdigest()==r['STL_sha256']
  if r['inherited_exact_V25']:
   assert f.read_bytes()==(v.V25/r['STL']).read_bytes()and g.read_bytes()==(v.V25/r['STEP']).read_bytes();mesh.append({'id':r['id'],'byte_identity_V25':True,'shape_proof_reused':'V25; ABS physical tolerances not qualified','passed':True});continue
  t=trimesh.load_mesh(f,process=True);s=cq.importers.importStep(str(g)).val();ok=bool(t.is_watertight and t.is_volume and t.is_winding_consistent and len(t.split())==1 and s.isValid()and len(s.Solids())==1 and min(t.bounds[0])>=-.001 and abs(t.volume-s.Volume())/s.Volume()<.01)
  mesh.append({'id':r['id'],'watertight':bool(t.is_watertight),'components':len(t.split()),'STEP_valid':bool(s.isValid()),'STEP_solids':len(s.Solids()),'relative_volume_error':abs(t.volume-s.Volume())/s.Volume(),'passed':ok});assert ok,r['id']
 save('maillages.json',mesh);print('STL/STEP reopens pass',len(mesh),flush=True)
 inst=[(q['label'],q['shape'])for q in v.INST]
 for(an,a),(bn,b)in itertools.combinations(inst,2):clear(an+' / '+bn,a,b)
 source=refs();print('own pairs',len(CHECKS),flush=True)
 for n,s in inst:
  for rn,rs in source:clear(n+' / '+rn,s,rs)
 # New base material and lid additions must not compromise real C05 geometry.
 meta=json.loads((P.parent/'V2_0_EMBOITEMENT/preuves/C05-collisions-G12p25.json').read_text());tx,ty=meta['Tx_Ty'];native=[]
 for q in meta['source_geometry']:
  a=q['bbox_ST'];native.append([a[0]+tx,a[1]+tx,a[2]-ty,a[3]-ty,a[4],a[5]])
 for n,old in [('01_base_ABS','01_base'),('02_couvercle_ARNACO_USER','02_couvercle_ARNACO')]:
  delta=v.PARTS[n]['shape'].cut(v.ref(old));b=bb(delta);candidates=[q for q in native if near(b,q)]
  # If global bbox overlaps, check individual solids of added material against source boxes.
  for s in delta.Solids():
   a=bb(s)
   for j,bn in enumerate(native):
    if near(a,bn):clear(f'added {n} / STbbox{j}',s,cq.Solid.makeBox(bn[1]-bn[0],bn[3]-bn[2],bn[5]-bn[4],cq.Vector(bn[0],bn[2],bn[4])))
 # Pusher operating example: trim selected tip to restgap0.3. First click0.4
 # user observation only; maxbuttonovertravel UNKNOWN, final stop tuned cold.
 ui=[];cage=v.PARTS['40_cage_USER_captive']['shape'];lid=v.PARTS['02_couvercle_ARNACO_USER']['shape']
 for H in [8.56,10.2]:
  for stroke in [0,.2,.7]:
   rod=v.pusher(H+.3).translate((0,0,-stroke))
   clear(f'USER H{H} stroke{stroke}/cage',rod,cage);clear(f'USER H{H} stroke{stroke}/lid',rod,lid)
   for rn,rs in source:clear(f'USER H{H} stroke{stroke}/{rn}',rod,rs)
   ui.append({'button_top_scenario':H,'restgap':.3,'stroke':stroke,'tip_Z':H+.3-stroke,'button_travel_after_gap':max(stroke-.3,0),'mechanical_first_click_observation_not_maxstroke':.4})
 # At MAX legitimate adjustment (1.4), the stem stays in8mm guide and flange
 # stays captive; it is NOT legitimate to use this stroke blindly on B1.
 for stroke in [0,1.4,2.0]:
  rod=v.pusher(10.5).translate((0,0,-stroke));clear(f'USER retention sweep{stroke}/cage',rod,cage);clear(f'USER retention sweep{stroke}/lid',rod,lid)
 # Standard upper/lower washers, supportheights and M2 ends.
 m2=[];assembl=cq.Assembly(name='V27_HARDWARE_DESIGN_ENVELOPES')
 for H in [10,10.6,11.2,11.8]:
  refsH=dict(refs(H))
  for kind in ['FRAM','SD']:
   s=v.saddle(kind,H);clear(f'saddle {kind} H{H}/shield',s,refsH['shield_actual_holes_RC_envelopes'])
   for rn,rs in refsH.items():clear(f'saddle {kind} H{H}/{rn}',s,rs)
   for j,(x,y)in enumerate(m.C['module_supports_'+kind]):
    t=1.6 if kind=='FRAM'else 1.57;top=13.85+H+t;uw=m.cyl(x,y,top,2.25,.3).cut(m.cyl(x,y,top-.1,1.1,.5));lw=m.cyl(x,y,11.95,2.25,.3).cut(m.cyl(x,y,11.9,1.1,.5));b=bolt(x,y,11.95)
    ns=[nut(x,y,top+.3+i*1.6,4,1.6)for i in [0,1]]
    for kn,a in [('upper_washer',uw),('lower_washer',lw),('M2',b),('nut0',ns[0]),('nut1',ns[1])]:
     clear(f'{kind}{j} H{H} {kn}/case',a,v.PARTS['01_base_ABS']['shape'])
     for rn,rs in refsH.items():clear(f'{kind}{j} H{H} {kn}/{rn}',a,rs)
     for pn,ps in inst:
      if pn.startswith('20_selle_'):continue
      clear(f'{kind}{j} H{H} {kn}/{pn}',a,ps)
     if H==11.2:assembl.add(a,name=f'{kind}{j}_{kn}')
    if kind=='SD':
     corridor=m.box(0,88.3,13.85+H+1,23.1,103.5,13.85+H+4)
     for kn,a in [('saddle',s),('M2',b),('washer',uw),('nuts',cq.Compound.makeCompound(ns))]:clear(f'SD {j} H{H} {kn}/extraction',a,corridor)
    m2.append({'kind':kind,'H':H,'head_bottom':9.75,'washer_height':.3,'nut_top':top+.3+3.2,'M2x30_L29p5_thread_emergence':11.95+29.5-(top+.3+3.2)})
 # Retainer guided pads and steel clearance at extremes, actual modules.
 pads=[]
 for B in [24.75,26.65,29.15]:
  rh=dict(refs(B-13.85-1.6))
  for which,fn,pnames in [('MKI','30_etrier_MKI_ABS',['32_patin_MKI_avant_ABS','33_patin_MKI_arriere_ABS']),('Click','31_etrier_Click_ABS',['34_patin_Click_ABS']*2)]:
   fr=v.PARTS[fn]['shape'];module=rh['Click_native'if which=='Click'else'MKI_Gerber_component_envelopes'];clear(f'{which} B{B}/native',fr,module)
   for i,((x,y),pn)in enumerate(zip(v.v.CONFIG[which]['guides'],pnames)):
    sh=v.PARTS[pn]['shape'].translate((x,-y,B));bs=bolt(x,y,B+13.5,12,1.5,3,3.2,False);ny=nut(x,y,32.8,5.5,4,1.5)
    for kn,a in [('pad',sh),('M3',bs),('DIN985',ny)]:
     clear(f'{which}{i} B{B}{kn}/frame',a,fr);clear(f'{which}{i} B{B}{kn}/module',a,module);clear(f'{which}{i} B{B}{kn}/lid',a,lid)
     if B==26.65:assembl.add(a,name=f'{which}{i}_{kn}')
    clear(f'{which}{i} B{B} bolt/pad',bs,sh);pads.append({'B':B,'which':which,'i':i,'head_top':B+16.7,'lid_margin':47-B-16.7})
 # Approved existing M3mounts except deleted doors/hatch; new cage uses same bolts.
 mounts=case_mounts()
 for j,(kn,x,y,z,u,a)in enumerate(mounts):
  p=(x,-y,z)
  for L in [11.5,12.5]:
   bs=cq.Solid.makeCylinder(1.5,L,cq.Vector(*p),cq.Vector(*u)).fuse(cq.Solid.makeCylinder(3,3.2,cq.Vector(*p),cq.Vector(*[-t for t in u])))
   ns=nut(0,0,0,5.5,2.4,1.5)
   if u==(0,0,-1):ns=ns.rotate((0,0,0),(1,0,0),180)
   else:ns=ns.rotate((0,0,0),(1,0,0),90)
   ns=ns.translate(tuple(p[k]+a*u[k]for k in range(3)))
   for bn,b in [('bolt',bs),('nut',ns)]:
    for pn,ps in inst:clear(f'M3{j}{kn}L{L}{bn}/{pn}',b,ps)
    for rn,rs in source:clear(f'M3{j}{kn}L{L}{bn}/{rn}',b,rs)
   if L==11.5:assembl.add(bs,name=f'case{j}_bolt');assembl.add(ns,name=f'case{j}_nut')
 # M2 frame anchors, standard lower washers.
 for which,fn in [('MKI','30_etrier_MKI_ABS'),('Click','31_etrier_Click_ABS')]:
  for j,(x,y)in enumerate(v.v.CONFIG[which]['anchors']):
   b=bolt(x,y,11.95);wa=m.cyl(x,y,11.95,2.25,.3).cut(m.cyl(x,y,11.9,1.1,.5));ns=[nut(x,y,34.5+i*1.6,4,1.6)for i in [0,1]]
   for kn,a in [('bolt',b),('washer',wa),('nut0',ns[0]),('nut1',ns[1])]:
    for pn,ps in inst:clear(f'anchor{which}{j}{kn}/{pn}',a,ps)
    for rn,rs in source:clear(f'anchor{which}{j}{kn}/{rn}',a,rs)
    assembl.add(a,name=f'anchor{which}{j}_{kn}')
 # Stop screw+nyloc and spring conservative annulus at rest/pressed.
 for z in [31.6,32.2,32.8]:
  x,y=v.STOP;b=bolt(x,y,z,12,1.5,3,3.2,True);ns=nut(x,y,34.6,5.5,4,1.5)
  clear(f'USERstop{z}/cage',b,cage);clear(f'USERstopnut{z}/cage',ns,cage)
  # Tip touches flange only at the associated hardstop, never through it.
  stroke=45-(z+12);rod=v.pusher(10.5).translate((0,0,-stroke));clear(f'USERstop{z}/rod',b,rod)
  if z==32.2:assembl.add(b,name='USER_stop');assembl.add(ns,name='USER_stop_nut')
 for stroke in [0,.7,1.4]:
  spring=m.cyl(v.UX,v.UY,36,3.525,9-stroke).cut(m.cyl(v.UX,v.UY,35.9,2.75,9.2-stroke));rod=v.pusher(10.5).translate((0,0,-stroke))
  clear(f'spring{stroke}/cage',spring,cage);clear(f'spring{stroke}/rod',spring,rod)
 # Cable paths are acceptance envelopes, NOT measurements of unknown cables.
 cable=m.box(61.7+2,-25,-2.5,79.7-2,-6,5.5);usbc=m.box(59.02,126.6,-2.5,75.02,151,5.5)
 rs=cq.Solid.makeCylinder(4,28,cq.Vector(144,-92.5,34.5),cq.Vector(1,0,0))
 sdslot=m.box(6,89.5,25,23.1,101.5,29.5)
 for name,s in [('STLINK14x8',cable),('CN15_16x8',usbc),('RS485_D8_with_protection',rs),('SD_extraction_12x4p5',sdslot)]:
  for pn,ps in inst:clear(name+'/'+pn,s,ps)
 # Sidepanel loading: move up off bottom tongue before placing cable, then lower
 # around alreadyconnected wire; open U must clear cable along entire path.
 ep=v.PARTS['06_passe_cable_RS485']['shape']
 for dz in [0,4,10,20,30]:clear(f'east panel loading dz{dz}/cable',ep.translate((0,0,dz)),rs)
 # Ribbon sweep inherited route but tested against new11x5U and ABS frames.
 for B in [24.75,26.65,29.15]:
  pts=np.array([[52.635,-33.385,B+8.5],[52.635,-17,B+10.5],[55.5,0,43],[55.5,22,43]])
  cv=[(1-t)**3*pts[0]+3*t*(1-t)**2*pts[1]+3*t*t*(1-t)*pts[2]+t**3*pts[3]for t in np.linspace(0,1,101)];sl=[]
  for a,b in zip(cv,cv[1:]):
   lo=np.minimum(a,b);hi=np.maximum(a,b);sl.append(m.box(lo[0]-4,-hi[1],lo[2]-.5,hi[0]+4,-lo[1],hi[2]+.5))
  sweep=cq.Compound.makeCompound(sl)
  for pn,ps in inst:clear(f'ribbon B{B}/{pn}',sweep,ps)
 assembl.save(str(P/'cad/quincaillerie-enveloppes.step'));v.trim(P/'cad/quincaillerie-enveloppes.step')
 user={'samples':ui,'upward_custody':'flange23 > lidbore13; no release path with cage installed','downward_custody':'two printedpinsZ43 plus normaladjustablestop43.6..44.8; flange cannot fall past guide','shaft_guide_radial_clearance':.4,'guide_length':18,'maximum_tilt_model_deg':math.degrees(math.atan(.8/18)),'flange_projected_min_d_at_tilt':23*math.cos(math.atan(.8/18)),'lid_hole_d':13,'head_recess':.6,'stop_operation_range':[.2,1.4],'shipping_stop_stroke':.2,'first_click_observed_approx_only':.4,'max_B1_overtravel_and_force':'UNKNOWN; adjust stop to actual first click at cold assembly, no guarantee from approximate observation','spring_example_rate_N_mm':.728,'spring_free_length':13.5,'spring_installed_range':[7.6,9],'spring_static_min_length_manufacturer':3.34,'spring_calculated_force_rest':.728*(13.5-9),'spring_calculated_force_at_max_stroke':.728*(13.5-7.6),'physical_return_force_and_ergonomics':'NOT_TESTED','no_firmware_change':True}
 save('USER-controle.json',user);save('M2-washers.json',m2);save('patins.json',pads);save('intersections.json',{'checks':CHECKS,'positive_intersections':HITS,'passed':not HITS})
 summary={'types':len(rows),'print_parts_two_kits':sum(r['quantity_to_print_two_kits']for r in rows),'useful_two_kits':sum(r['useful_two_kits']for r in rows),'all_new_meshes_passed':True,'unchanged_exact_shapes_reused':sum(r['inherited_exact_V25']for r in rows),'BREP_checks':len(CHECKS),'positive_intersections':HITS,'C05_native_source_solids':len(native),'physical_qualification':False,'passed':not HITS}
 save('synthese-controles.json',summary);print(json.dumps(summary,indent=2),flush=True);assert not HITS,'Review actual intersections'
if __name__=='__main__':main()
