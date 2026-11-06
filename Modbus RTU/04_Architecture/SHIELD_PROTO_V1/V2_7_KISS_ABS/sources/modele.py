"""V27 targeted ABS revision. X=KiCad X, Y=-KiCad Y, Z0=Nucleo top, mm.

Supplier solids never exported. Geometry inherited from V25/V26 remains read-only.
"""
from pathlib import Path
import importlib.util,json,math,hashlib,shutil
import cadquery as cq
P=Path(__file__).resolve().parents[1];R=P.parent;V26=R/'V2_6_IMPRESSION_COMPLETE';V25=R/'V2_5_KISS_FIXATIONS'
def load(n,p):
 sp=importlib.util.spec_from_file_location(n,p);o=importlib.util.module_from_spec(sp);sp.loader.exec_module(o);return o
v=load('v26_geometry',V26/'sources/modele.py');m=v.m;box=m.box;cyl=m.cyl;capsule=m.capsule
UX,UY=m.C['USER']['xy'];BOSSES=[[15.8,131],[44,131]];STOP=[UX,UY+9];PARTS={};INST=[]
CONTACTS={'MKI':[[48,23.6],[48,52.3]],'Click':[[84.4,86.75],[84.4,96.75]]}
def part(n,s,q,use,role,ori='flat',inherited=False):
 s=s.clean();assert s.isValid()and len(s.Solids())==1,(n,'invalid or disconnected')
 PARTS[n]={'shape':s,'qty':q,'use':use,'role':role,'orientation':ori,'inherited_exact':inherited};return s
def put(n,s=None,label=None):INST.append({'id':n,'label':label or n,'shape':PARTS[n]['shape']if s is None else s})
def ref(n):return cq.importers.importStep(str(V25/'cad'/(n+'.step'))).val()
def casebase():
 s=ref('01_base')
 # Close only the obsolete south and west panel bays, no electronic move.
 s=s.fuse(box(20,143,-7,90,146,47),box(8,74,17,11,118,47))
 # Close the original south USER and oversized USB access in a fixed wall.
 s=s.cut(box(57.02,142,-3,77.02,147,7))
 # Fixed recessed SD finger pocket, narrow rear slit. No printed door.
 s=s.fuse(box(8,84,20.5,18,108,35.5))
 s=s.cut(box(7,86,22,16,106,34)).cut(box(15.9,88,24,18.2,104,30.5))
 # Existing east cassette retained as one small cable-specific panel.
 # Its lower U is closed by this integral tongue during vertical assembly.
 s=s.fuse(box(161.6,88.3,21,164.4,96.7,30.2))
 # ABS allowance on captive flats, without altering the metal reference.
 for pts,top in [(m.C['lid_screw_centres'],47),(m.C['case_keeper_pivots'],1.4),([[154,82],[154,101]],25)]:
  for x,y in pts:s=s.cut(m.hexhole(x,y,top-5.3,6.1,3.0))
 return s.clean()
def north():
 s=ref('03_cassette_nord')
 s=s.fuse(box(55,-18.2,-5,84,-15.8,10),box(46,-18.2,32,61,-15.8,46))
 s=s.cut(box(61.7,-19,-3,79.7,-15,6.5))
 # Open top permits installing the preassembled IDC; lid closes the U.
 s=s.cut(box(50,-19,42,61,-15,48));return s.clean()
def east():
 s=ref('06_cassette_RS485').fuse(box(161.8,80,25,164.2,104,42.5))
 # Cable U open downward. Place cable on base tongue, slide panel around it.
 s=s.cut(box(161,88,20,165,97,34.5))
 cut=cq.Solid.makeCylinder(4.5,5,cq.Vector(161,-92.5,34.5),cq.Vector(1,0,0));return s.cut(cut).clean()
def lid():
 s=ref('02_couvercle_ARNACO')
 s=s.fuse(box(131,78,47,145,107,50),box(124,74,48,153,111,50))
 for x in [127.5,149.5]:s=s.fuse(cyl(x,92.5,47,1.9,3))
 s=s.cut(cyl(UX,UY,46.8,6.5,4))
 for x,y in BOSSES:s=s.cut(cyl(x,y,46.8,1.8,4))
 return s.clean()
def user_cage():
 s=cyl(UX,UY,18,5,18)
 for x,y in BOSSES:
  s=s.fuse(capsule([UX,UY],[x,y],3,18,3),cyl(x,y,18,4.5,29))
  # Nut head-to-entry7.4mm, fully engaged with M3x12 from lid top.
  s=s.cut(m.hexhole(x,y,40.2,6.1,2.8)).cut(box(x-6,y-3.2,40.2,x,y+3.2,43))
  s=s.cut(cyl(x,y,27.8,1.8,20))
 x,y=STOP
 s=s.fuse(capsule([UX,UY],[x,y],3,18,3),cyl(x,y,18,4.5,22.2))
 for dx in [-8,8]:s=s.fuse(capsule([UX,UY],[UX+dx,UY],2.2,18,3),cyl(UX+dx,UY,21,1.8,22))
 # Capture the lower spring end radially; no additional printed piece.
 s=s.fuse(cyl(UX,UY,36,5,1.2)).cut(cyl(UX,UY,36,3.75,1.4))
 # Stop screw points UP, accessible from below removed lid. DIN985 locks it.
 s=s.cut(cyl(x,y,17.8,3.3,15.2)).cut(cyl(x,y,32.8,1.8,8))
 s=s.cut(m.hexhole(x,y,34.4,6.1,4.6)).cut(box(x-5,y-3.2,34.4,x,y+3.2,39))
 s=s.cut(cyl(UX,UY,17.8,2.4,18.5))
 return s.clean()
def pusher(tip):
 # Top head fits13mm lid bore,23mm flange cannot pass. Shaft4 guided4.8.
 s=cyl(UX,UY,tip,2,45-tip).fuse(cyl(UX,UY,45,11.5,2),cyl(UX,UY,47,5.5,2.4),cyl(UX,UY,43.8,2.6,1.2))
 if tip in [8.8,9.6,10.4]:
  for j in range([8.8,9.6,10.4].index(tip)+1):s=s.cut(cyl(UX-2+j*2,UY,48.9,.6,.7))
 return s.clean()
def saddle(which,H):
 pts=m.C['module_supports_FRAM'if which=='FRAM'else'module_supports_SD'];s=None
 for x,y in pts:
  q=cq.Workplane('XY').rect(4.4,4).extrude(H).edges('|Z').fillet(.4).val().translate((x,-y,13.85))
  s=q if s is None else s.fuse(q)
 if which=='FRAM':route=[pts[0],[45,75.6],[65.32,75.6],pts[1]]
 else:route=[pts[0],[26.6,85.68],[26.6,106],pts[1]]
 for a,b in zip(route,route[1:]):s=s.fuse(capsule(a,b,1.2,13.85,2))
 for x,y in pts:s=s.cut(cyl(x,y,13.7,1.2,H+.3))
 if H in [10.6,11.2,11.8]:
  for j in range([10.6,11.2,11.8].index(H)+1):
   x,y=(51+4*j,75.6)if which=='FRAM'else(26.6,91+4*j)
   s=s.cut(cyl(x,y,15.35,.6,.7))
 return s.clean()
def frame(which):
 s=v.frame(which)
 for i,(x,y)in enumerate(v.CONFIG[which]['guides']):
  k='front'if which=='MKI'and i==0 else'rear'if which=='MKI'else'straight'
  s=s.fuse(box(x-4.5,y-4.5,23.5,x+4.5,y+4.5,38));s=v.guide(x,y,k,s)
  s=s.cut(box(x-2.8,y-2.8,23.4,x+2.8,y+2.8,32)).cut(m.hexhole(x,y,32.2,6.1,4.6))
  # Broader guides need exits that reach their new outside faces; no0.2mm skin.
  if k=='front':s=s.cut(box(x-1.5,y+2.5,23.4,x+3.9,y+4.7,32.2))
  elif k=='rear':s=s.cut(box(x-1.5,y+2.5,23.4,x+1.5,y+4.7,32.2))
  else:s=s.cut(box(x+2.5,y-1.5,23.4,x+4.7,y+1.5,32.2))
 for x,y in v.CONFIG[which]['anchors']:s=s.cut(cyl(x,y,34.5,2.7,10)).cut(cyl(x,y,13.7,1.2,29))
 return s.clean()
def shoe(kind):
 s=box(-2.5,-2.5,0,2.5,2.5,2.5)
 if kind=='front':
  beam=capsule([0,0],[4,7.6],1.1,0,2.5).cut(box(-2,6.0,-.1,7,10,1));toe=box(2.8,7.1,0,5.2,8.1,1.3)
 elif kind=='rear':
  beam=box(-1,0,0,1,7.5,2.5).fuse(box(0,5.5,0,10.7,7.5,2.5),box(8.3,3.8,1,10.7,6.5,2.5));toe=box(8.3,3.3,0,10.7,4.3,1.3)
 else:
  beam=box(0,-1.2,1,6.6,1.2,2.5);toe=box(5.6,-1.2,0,6.6,1.2,1.3)
 return s.fuse(beam,toe).cut(cyl(0,0,1.5,1.7,1.2)).clean()
def coupon():
 s=box(0,0,0,50,16,2)
 # Bore/hex/slider examples; no tiny individual discs or altered D126.
 for j,d in enumerate([2.4,3.6,4.8,13]):s=s.cut(cyl(5+j*11,8,-.1,d/2,2.2))
 s=s.fuse(box(0,0,2,10,8,6)).cut(m.hexhole(5,4,2,6.1,4.2))
 s=s.fuse(box(0,11,2,7,16,12.25),box(45,0,2,50,8,11.2))
 return s.clean()
def build():
 part('01_base_ABS',casebase(),2,2,'Caisse, CN15 fixe et poche SD intégrés')
 part('02_couvercle_ARNACO_USER',lid(),2,2,'Capot fermé sans trappe bornier; USER encastré Ø13')
 part('03_facade_STLINK_LED_nappe',north(),2,2,'Une cassette nord, fente nappe en U, vitre conservée','north')
 part('06_passe_cable_RS485',east(),2,2,'Un panneau de passage fixe, pas une trappe','east')
 for n,q,use in [('07_cadre_vitre',2,2),('10_serre_nappe',2,2),('13_D126',8,6),('15_retenue_Nucleo',6,6)]:
  old=next(r for r in json.loads((V25/'preuves/pieces.json').read_text())if r['id']==n)
  part(n,ref(n),q,use,'Forme V25 inchangée; ABS à contrôler à froid',old['orientation'],True)
 bar=ref('11_serre_cable_RS485')
 for y in [87.1,96.7]:bar=bar.cut(box(152.5,y,26.8,155.5,y+1.2,30.2))
 part('11_serre_cable_RS485',bar,2,2,'Deux fentes Colson, garniture souple et câble placé sur la bride')
 for which in ['FRAM','SD']:
  for h in [10.6,11.2,11.8]:part('20_selle_'+which+'_H'+str(h).replace('.','p'),saddle(which,h),2,2 if h==11.2 else 0,'Une selle deux portées, ponçage à plat≤0,6mm; choisir UNE hauteur/module')
 for w,n in [('MKI','30_etrier_MKI_ABS'),('Click','31_etrier_Click_ABS')]:part(n,frame(w),2,2,'Guide élargi ABS; contact uniquement patins isolants','upside_down')
 for k,n,q in [('front','32_patin_MKI_avant_ABS',2),('rear','33_patin_MKI_arriere_ABS',2),('straight','34_patin_Click_ABS',4)]:part(n,shoe(k),q,q,'Patin contact1,0×2,4, coupelle fermée1,5','upside_down')
 part('40_cage_USER_captive',user_cage(),2,2,'Cage fixée sous capot, guide et butées, aucun élément libre')
 for tip in [8.8,9.6,10.4]:part('41_USER_pointe_Z'+str(tip).replace('.','p'),pusher(tip),2,2 if tip==10.4 else 0,'Choisir UNE tige; ajustage pointe hors banc, jeu au repos obligatoire','upside_down')
 part('50_coupon_ABS',coupon(),1,0,'Coupon unique pour profil machine; ne pas intégrer au TR2')
 # One actual nominal assembly; alternatives excluded.
 for n in ['01_base_ABS','02_couvercle_ARNACO_USER','03_facade_STLINK_LED_nappe','06_passe_cable_RS485','07_cadre_vitre','10_serre_nappe','11_serre_cable_RS485','40_cage_USER_captive','41_USER_pointe_Z10p4','20_selle_FRAM_H11p2','20_selle_SD_H11p2','30_etrier_MKI_ABS','31_etrier_Click_ABS']:put(n)
 for j,(x,y)in enumerate(m.C['anchors']):put('13_D126',PARTS['13_D126']['shape'].rotate((0,0,0),(0,0,1),180 if j==0 else 0).translate((x,-y,0)),f'D126_{j}')
 # Exact established keeper placements from V25 assembly generator.
 old=load('v25_revision',V25/'sources/revision.py');items,_=old.assembly()
 for r in items:
  if r['part']=='15_retenue_Nucleo':put(r['part'],r['shape'],r['label'])
 B=13.85+11.2+1.6
 for which in ['MKI','Click']:
  for i,((x,y),n)in enumerate(zip(v.CONFIG[which]['guides'],['32_patin_MKI_avant_ABS','33_patin_MKI_arriere_ABS']if which=='MKI'else['34_patin_Click_ABS']*2)):put(n,PARTS[n]['shape'].translate((x,-y,B)),which+'_patin'+str(i))
 return PARTS

def trim(f):
 b=f.read_bytes();f.write_bytes(b'\n'.join(q.rstrip()for q in b.splitlines())+b'\n')
def main():
 build();rows=[];a=cq.Assembly(name='V27_OWN_ASSEMBLY_NOMINAL_NOT_PHYSICAL_QUALIFICATION')
 for r in INST:a.add(r['shape'],name=r['label'])
 for n,r in PARTS.items():
  sf=P/'stl'/(n+'.stl');cf=P/'cad'/(n+'.step');pr=m.normalized(r['shape'],r['orientation'])
  if r['inherited_exact']:
   shutil.copyfile(V25/'stl'/(n+'.stl'),sf);shutil.copyfile(V25/'cad'/(n+'.step'),cf)
  else:cq.exporters.export(pr,str(sf),tolerance=.04,angularTolerance=.12);cq.exporters.export(r['shape'],str(cf));trim(cf)
  b=pr.BoundingBox();rows.append({'id':n,'quantity_to_print_two_kits':r['qty'],'useful_two_kits':r['use'],'variant_or_spare':r['qty']-r['use'],'role':r['role'],'orientation':r['orientation'],'inherited_exact_V25':r['inherited_exact'],'STL':'stl/'+sf.name,'STEP':'cad/'+cf.name,'STL_sha256':hashlib.sha256(sf.read_bytes()).hexdigest(),'dimensions_print_mm':[b.xlen,b.ylen,b.zlen],'volume_mm3':r['shape'].Volume()})
 a.save(str(P/'cad/assemblage-propre.step'));trim(P/'cad/assemblage-propre.step')
 params={'mission_id':'TR2-20261106-SHIELD-V27-KISS-ABS-MORVAN','material':'ABS non-conductive','G':12.25,'frame':'KiCadX,-KiCadY,Z0Nucleo','USER':{'xy':[UX,UY],'reported_button_top':10.2,'first_click_reported_approx':.4,'shaft_d':4,'guide_bore':4.8,'head_d':11,'lid_bore_d':13,'flange_d':23,'head_top_rest':49.4,'lid_top':50,'flange_rest_bottom':45,'guide_spring_seat_Z':36,'stop_xy':STOP,'stop_adjustable_Z':[43.6,44.8],'operating_travel_range':[.2,1.4],'printed_fail_safe_Z':43,'spring_rest_length':9,'spring_min_operating_length':7.6,'spring_example':'Gutekunst DIN2098 D-064,13.5free,OD6.8,ID5.8,rate0.728N/mm','pusher_tip_variants':[8.8,9.6,10.4],'cold_fit_required':True,'button_force_max_overtravel':None},'USER_cage_lid_mounts':BOSSES,'support_heights':[10.6,11.2,11.8],'sanding_max':.6,'mechanical_support_range':[10,11.8],'electrical_engagement_window':'not known; manual socket stops first','washer_standard':'DIN433 nylon M2,2.2x4.5x0.3','separate_washers_two_kits':24,'module_contacts':CONTACTS,'ABS':{'slider_clearance_per_side':.3,'nyloc_pocket_AF':6.1,'case_scale':1,'shrinkage_compensation':'machine specific, not applied blindly to critical ST distances'},'case_outside':[158,166,64],'physical_qualification':False}
 (P/'sources/parametres.json').write_text(json.dumps(params,indent=2,ensure_ascii=False)+'\n');(P/'preuves/pieces.json').write_text(json.dumps(rows,indent=2,ensure_ascii=False)+'\n')
 print(json.dumps({'types':len(rows),'pieces':sum(r['qty']for r in PARTS.values()),'useful':sum(r['use']for r in PARTS.values()),'own_assembly_instances':len(INST)},indent=2),flush=True)
if __name__=='__main__':main()
