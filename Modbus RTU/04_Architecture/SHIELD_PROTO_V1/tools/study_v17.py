"""New V1.7 study outputs only. Never import the V1.5 generator (it writes outputs).
Actual C05 LED/B1 centres remain null. No PCB/firmware or old CAD saved.
"""
from pathlib import Path
import json, math, hashlib, subprocess, itertools
import numpy as np
import cadquery as cq
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from matplotlib.backends.backend_pdf import PdfPages
from mpl_toolkits.mplot3d.art3d import Poly3DCollection

P=Path(__file__).resolve().parents[1];V=P/'mechanical/V1_7';O=V/'REVIEW_ONLY';Q=P/'docs/ETUDE_VITRAGE_USER_V1_7';O.mkdir(exist_ok=True);Q.mkdir(exist_ok=True)
C=json.loads((V/'parameters.json').read_text());old=json.loads((P/'mechanical/V1_5/parameters.json').read_text())
BASE='39c33c69a502d19c6590adffac0ccf847d9f0e34'
assert all(v is None for v in C['actual_C05_centres'].values())
def save(name,obj): (Q/name).write_text(json.dumps(obj,ensure_ascii=False,indent=2)+'\n')
def box(a,b):
 x,y,z=a;xx,yy,zz=b
 return cq.Workplane('XY').box(xx-x,yy-y,zz-z,centered=False).translate((x,-yy,z)).val()
def cyl(x,y,z,r,h):return cq.Workplane('XY').circle(r).extrude(h).translate((x,-y,z)).val()
def hit(a,b,lo,hi):
 t0,t1=0.,1.
 for x,d,l,h in zip(a,np.array(b)-a,lo,hi):
  if abs(d)<1e-12:
   if x<l or x>h:return False
  else:
   u,v=sorted(((l-x)/d,(h-x)/d));t0=max(t0,u);t1=min(t1,v)
   if t0>t1:return False
 return True
boxes={
 'shield':[[20,17,-1.6],[110,117,0]],
 'MKI_provisional':[[42.46,21.46,11],[62.78,54.48,21]],
 'ribbon_reservation':[[46,-14,9],[61,21,22]],
 'MKI_retainer_roof_conservative':[[36,17,22],[74,57.5,25]],
 'MKI_post_1':[[36,17,0],[42,23,25]],'MKI_post_2':[[68,51.5,0],[74,57.5,25]],
 'J8_conservative':[[43.3,22,0],[62,56,11]],
 'STLINK_port_reservation_NOT_actual':[[55,-12,-17],[81,5,1]]}
domain=C['scenario']['source_domain_NOT_LED_positions'];reserve=C['scenario']['required_obstacle_reserve_mm']
sources=[np.array(a) for a in itertools.product(np.linspace(45,65,11),np.linspace(12,28,9),[-12.6,-11.1,-9.6])]
eyes=[np.array(a) for a in itertools.product([-40,0,40],[-160,-90],[-10,-5])]
results=[];failures={k:0 for k in boxes};allowed_by_source=[]
for a in sources:
 accepted=0
 for e in eyes:
  crossings=[a+(e-a)*(y-a[1])/(e[1]-a[1]) for y in [-20,-17]]
  ap=C['window_candidate']['aperture_xz_mm'];in_window=all(np.all(q[[0,2]]>=ap[0]) and np.all(q[[0,2]]<=ap[1]) for q in crossings)
  obstruction=[name for name,(lo,hi) in boxes.items() if hit(a,e,np.array(lo)-reserve,np.array(hi)+reserve)]
  for name in obstruction:failures[name]+=1
  angle=math.degrees(math.atan2(abs(e[2]-a[2]),np.linalg.norm((e-a)[:2])))
  ok=in_window and not obstruction
  if ok:accepted+=1
  results.append({'source_design_domain_xyz':a.tolist(),'eye_xyz':e.tolist(),'angle_deg':angle,'in_aperture':in_window,'obstructions':obstruction,'conditional_clear':ok})
 allowed_by_source.append({'source_xyz_NOT_LED':a.tolist(),'clear_eyes':accepted})
ok=[v for v in results if v['conditional_clear']]
wall_angle=math.degrees(math.atan((12-(-20))/(37-(-9.6))))
assert wall_angle>30
save('optique.json',{'status':'CONDITIONAL_SCAN_NOT_C05_VALIDATION','source_domain_NOT_LED_positions':domain,'eyes_region_mm':[[-40,-160,-10],[40,-90,-5]],'eyes':len(eyes),'sources':len(sources),'rays':len(results),'conditional_clear_rays':len(ok),'conditional_clear_angles_from_horizontal_deg':None if not ok else [min(v['angle_deg'] for v in ok),max(v['angle_deg'] for v in ok)],'source_points_with_some_clear_view':sum(v['clear_eyes']>0 for v in allowed_by_source),'source_points_clear_for_all_eyes':sum(v['clear_eyes']==len(eyes) for v in allowed_by_source),'per_eye_clear_source_points':[{'eye':e.tolist(),'clear_sources':sum(v['conditional_clear'] and v['eye_xyz']==e.tolist() for v in results)} for e in eyes],'obstacle_reserve_mm':reserve,'obstacle_boxes':boxes,'obstruction_counts_nonexclusive':failures,'side_wall_Y_mm':-20,'old_near_normal_interpretation_REJECTED_after_user_correction':True,'side_wall_min_angle_from_vertical_for_previously_assumed_top_view_deg':wall_angle,'individual_source_scan':allowed_by_source,'photometry_and_real_C05':False,'notes':['no three LD positions assigned','point rays only, finite source and refraction not qualified','failure over hypothetical domain is not actual LED obstruction proof']})

U=C['USER_principle'];nt=-C['scenario']['G_Zio_mm']-C['scenario']['shield_thickness_mm'];bt=nt+C['observations_user']['B1_top_above_Nucleo_mm'];tip=bt+U['rest_gap_target_mm'];lid=C['scenario']['lid_z_mm'][0]
glo,ghi=U['rest_gap_scenario_mm'];slo,shi=U['external_stroke_scenario_mm'];alo,ahi=U['activation_travel_scenario_NOT_tolerance_mm']
budget={'Nucleo_top_z':nt,'B1_top_z_from_user_height':bt,'rest_contact_tip_z':tip,'lid_inner_to_tip_mm':lid-tip,
 'external_travel_to_first_click_nominal_mm':.6+.4,'remaining_spring_compression_at_nominal_click_and_stop_mm':1.6-.6-.4,
 'remaining_compression_scenario_min_mm':slo-ghi-ahi,'remaining_compression_scenario_max_mm':shi-glo-alo,
 'worst_spring_compression_if_B1_does_not_move_mm':shi-glo,'spring_travel_reserve_mm':1.5-(shi-glo),
 'gap_actual_must_be_adjusted_not_print_tolerance':True,'force_conditions':['F0+k*(s_min-g_max-a_max) >= F_click_max, with friction allowance','F0+k*(s_max-g_min) <= F_allowed_min','compression_max < spring_available_travel; no hard bottom or coil bind'],
 'F0_and_k_unknown_not_qualified':True,'B1_MPN_force_curve_and_total_travel_unknown':True}
assert budget['remaining_compression_scenario_min_mm']>0 and budget['spring_travel_reserve_mm']>.2
save('chaine_USER.json',budget)

# New editable mechanical STUDY parts; all unknown positions labelled explicitly.
parts=[]
def add(name,shape,color):
 assert shape.isValid() and shape.Volume()>0,name
 parts.append((name,shape,color))
geom=json.loads((P/'docs/pcb_geometry.json').read_text())
shield=box([20,17,-1.6],[110,117,0]);shield=shield.cut(cq.Compound.makeCompound([cyl(q['x'],q['y'],-1.7,q['drill']/2,1.8) for q in geom['pads']]))
add('Shield_V14_outline_holes_UNCHANGED',shield,(.1,.4,.2))
add('Nucleo_REFERENCE_not_C05',box([21.75,-6.75,-14.2],[91.75,126.59,-12.6]),(.15,.3,.6))
for n in ['MKI_provisional','ribbon_reservation','MKI_retainer_roof_conservative','MKI_post_1','MKI_post_2','STLINK_port_reservation_NOT_actual']:
 add(n,box(*boxes[n]),(.7,.4,.1) if 'MKI' in n else (.7,.2,.6))
w=C['window_candidate'];pane=box([27,-22.1,-13],[47,-20.6,-1])
frame=box([24,-24,-16],[50,-20,2]).cut(box([29,-24.1,-11],[45,-19.9,-3])).cut(box([26.8,-22.1,-13.2],[47.2,-19.9,-.8]))
gasket=box([27,-20.6,-13],[47,-20,-1]).cut(box([29,-20.7,-11],[45,-19.9,-3]))
patch=box([22,-20,-18],[52,-17,4]).cut(box([29,-20.1,-11],[45,-16.9,-3]))
for x,z in w['screw_centres_xz_mm']:
 cutter=cq.Solid.makeCylinder(1.1,8,cq.Vector(x,25,z),cq.Vector(0,-1,0))
 frame=frame.cut(cutter);patch=patch.cut(cutter)
add('North_wall_STUDY_patch_NOT_current_case',patch,(.6,.6,.6));add('PC_clear_plate_1p5',pane,(.5,.8,.9));add('Frame_removable',frame,(.3,.3,.3));add('Gasket_compressed_scenario',gasket,(.1,.1,.1))
# USER local principle at a deliberately hypothetical axis; actual B1 remains null.
x,y=U['illustration_axis_NOT_B1_xy'];plate=cyl(x,y,37,10,3).cut(cyl(x,y,36.9,2.2,3.2))
guide=cyl(x,y,0,5,4).cut(cyl(x,y,-.1,2.2,4.2)).fuse(cyl(x,y,33,5,4).cut(cyl(x,y,32.9,2.2,4.2)))
for dx in [-4.5,4.5]:guide=guide.fuse(box([x+dx-.5,y-4,4],[x+dx+.5,y-2,37]))
# Flange lower face at6; fixed lower bearing top4 -> stroke1.6 using stop ring top4.4.
guide=guide.fuse(cyl(x,y,4,5,.4).cut(cyl(x,y,3.9,2.2,.6)))
guide=guide.fuse(cyl(x,y,7,5,1).cut(cyl(x,y,6.9,2.2,1.2))) # upper capture face, not a return spring
rod=cyl(x,y,1,2,41).fuse(cyl(x,y,6,3,1),cyl(x,y,42,5,2)).cut(cyl(x,y,.9,1.6,5.1))
shoe=cyl(x,y,tip,1.5,4.6);spring_reservation=cyl(x,y,3,1.3,2.5) # envelope, no spring rate inferred
add('USER_local_plate_NOT_B1_XY',plate,(.6,.6,.6))
# Two opening halves allow assembly around the captive flange; screw detail still pending.
for suffix,half in [('left',box([x-6,y-6,-1],[x,y+6,38])),('right',box([x,y-6,-1],[x+6,y+6,38]))]:
 add('USER_guide_split_'+suffix,guide.intersect(half),(.4,.4,.4))
add('USER_carrier_tip_adjustment_interface_unthreaded',rod,(.8,.8,.7));add('USER_floating_shoe_soft_tip_principle',shoe,(.2,.7,.5));add('USER_spring_ENVELOPE_not_selected',spring_reservation,(.8,.3,.3))
# Real load and adjustment details are unresolved; this is not an STL manufacturing release.
asm=cq.Assembly(name='V17_STUDY_NO_REAL_LED_B1_POSITIONS')
for n,s,c in parts:asm.add(s,name=n,color=cq.Color(*c))
asm.export(str(O/'v17-study.step'))
usr=cq.Assembly(name='USER_LOCAL_PRINCIPLE_NOT_ACTUAL_C05_XY')
for n,s,c in parts:
 if n.startswith('USER_'):usr.add(s,name=n,color=cq.Color(*c))
usr.export(str(O/'user-principle.step'))
window=cq.Assembly(name='WINDOW_CANDIDATE_NO_OPTICAL_APPROVAL')
for n,s,c in parts:
 if n in ['North_wall_STUDY_patch_NOT_current_case','PC_clear_plate_1p5','Frame_removable','Gasket_compressed_scenario']:window.add(s,name=n,color=cq.Color(*c))
window.export(str(O/'window-principle.step'))
for f in O.glob('*.step'):
 raw=f.read_bytes();new=b'\n'.join(l.rstrip(b' \t\r') for l in raw.splitlines())+b'\n';assert raw.split()==new.split();f.write_bytes(new)
# Nominal mechanical checks at rest/stop, distinct from actuation force qualification.
rod_down=rod.translate((0,0,-1.6));assert rod.intersect(guide).Volume()<1e-7 and rod_down.intersect(guide).Volume()<1e-7
assert rod.intersect(plate).Volume()<1e-7 and rod_down.intersect(plate).Volume()<1e-7
assert (42-40)-1.6>=.4-1e-9 # head must not hit panel before the structural stop
assert frame.intersect(pane).Volume()<1e-7 and frame.intersect(gasket).Volume()<1e-7
step_read=[]
for f in O.glob('*.step'):
 s=cq.importers.importStep(str(f)).val();assert s.isValid() and s.Volume()>0
 step_read.append({'path':f.name,'solids':len(s.Solids()),'volume':s.Volume()})
save('CAO-controles.json',{'STEP_reopened':step_read,'parts':len(parts),'rod_guide_rest_and_stop_volume_overlap_zero':True,'head_panel_rest_and_stop_volume_overlap_zero':True,'head_to_panel_clearance_at_stop_mm':.4,'frame_plate_gasket_overlap_zero':True,'physical_fit':False,'spring_and_adjustment_threads_not_manufacturing_model':True})
save('interfaces-nord.json',{'window_frame_xz':[[24,-16],[50,2]],'STLINK_existing_blank_insert_xz':[[52,-20],[84,4]],'ribbon_existing_blank_insert_xz':[[41,4],[66,25]],'frame_to_STLINK_insert_horizontal_gap_mm':2,'frame_to_ribbon_insert_vertical_gap_mm':2,'aperture_to_STLINK_reserved_port_horizontal_gap_mm':10,'status':'NOMINAL_V15_ENVELOPES_ONLY_NOT_REAL_PLUG_FIT'})

plt.rcParams.update({'font.size':9,'svg.fonttype':'none'});figs=[]
fig=plt.figure(figsize=(11.7,8.3));figs.append(fig);ax=fig.add_subplot(111,projection='3d')
for n,s,c in parts:
 v,f=s.tessellate(.6,.5);v=np.array([[q.x,-q.y,q.z]for q in v]);polys=v[np.array(f)]
 ax.add_collection3d(Poly3DCollection(polys,facecolor=c,edgecolor='none',alpha=.35 if 'ribbon' in n or 'REFERENCE' in n else .75))
ax.set(xlim=(20,115),ylim=(-25,135),zlim=(-20,55),xlabel='X KiCad mm',ylabel='Y KiCad mm',zlabel='Z mm')
ax.view_init(elev=28,azim=-62);ax.set_box_aspect((95,160,75));ax.set_title('V1.7 : dérivé des dimensions V1.5 — fenêtre et USER de principe, positions C05 inconnues')
fig.text(.02,.03,'Les corps transparents sont des réservations. Aucun composant LED/B1 réel placé ; USER illustré sur un axe hypothétique.')
fig.tight_layout()
fig,ax=plt.subplots(figsize=(11.7,8.3));figs.append(fig)
for name,(lo,hi) in boxes.items():
 if name in ['shield','MKI_provisional','ribbon_reservation','STLINK_port_reservation_NOT_actual']:ax.add_patch(Rectangle((lo[1],lo[2]),hi[1]-lo[1],hi[2]-lo[2],alpha=.25,label=name))
ax.plot([-20,146],[37,37],color='black',label='Couvercle V1.5')
ax.plot([-20,-20],[-24,37],color='black',lw=3,label='Paroi ST-LINK verticale')
ax.plot([-20,-20],[-11,-3],color='blue',lw=5,label='Vitre latérale candidate, X29..45')
for row in ok[::max(1,len(ok)//4)][:4]:
 a=np.array(row['source_design_domain_xyz']);e=np.array(row['eye_xyz']);q=a+(e-a)*(-20-a[1])/(e[1]-a[1]);ax.plot([a[1],q[1]],[a[2],q[2]],'--',color='green')
ax.set(xlim=(-30,65),ylim=(-20,55),xlabel='Y KiCad mm',ylabel='Z mm',title='Coupe Y/Z : vitrage latéral sous shield ; projection seule non probante')
ax.text(-28,48,f'{len(results)} rayons / {len(eyes)} observateurs ; {len(ok)} passages conditionnels, pas trois LED identifiées.')
ax.text(-28,43,'Visée quasi horizontale confirmée par utilisateur ; passage en X à côté de ST-LINK à vérifier.')
ax.legend(loc='upper right',bbox_to_anchor=(1,.6),fontsize=8);ax.grid(alpha=.2);fig.tight_layout()
fig,ax=plt.subplots(figsize=(11.7,8.3));figs.append(fig)
ax.add_patch(Rectangle((-10,37),20,3,color='grey',label='Panneau/structure'))
ax.add_patch(Rectangle((-2.2,0),4.4,37,fill=False,ls='--',label='Guidage Ø4,4 / tige Ø4'))
ax.plot([-2,-2,2,2],[1,42,42,1],color='blue');ax.add_patch(Rectangle((-5,42),10,2,color='blue',alpha=.5,label='Carrier + tête (principe)'))
ax.add_patch(Rectangle((-5,4),10,.4,color='grey'));ax.add_patch(Rectangle((-3,6),6,1,color='blue',alpha=.5))
ax.add_patch(Rectangle((-5,7),10,1,color='grey',alpha=.5))
ax.add_patch(Rectangle((-1.5,tip),3,4.6,color='green',alpha=.5,label='Patin flottant / pointe souple'))
ax.add_patch(Rectangle((-1.3,3),2.6,2.5,fill=False,color='red',label='Ressort : enveloppe, k/F0 inconnus'))
ax.plot([-9,9],[bt,bt],color='green',label='Sommet B1 : z−2,4 au scénario G11')
ax.plot([-9,9],[nt,nt],color='black',label='Dessus Nucleo z−12,6')
ax.annotate('Jeu cible0,6 réglé au montage',xy=(0,tip),xytext=(9,-1),arrowprops={'arrowstyle':'->'})
ax.annotate('Butée structurelle : course1,6\nréaction dans le panneau, pas dans B1',xy=(3,5),xytext=(10,8),arrowprops={'arrowstyle':'->'})
ax.text(-17,48,'H B1=10,2 (mesuré) ; clic≈0,4 (observé). Réglage axial proposé±2mm ; centrage XY non acquis.')
ax.text(-17,45,'F0+k×1,25 ≤ F admissible inconnu ; réserve de ressort proposée1,5mm. Effort non qualifié.')
ax.set(xlim=(-18,45),ylim=(-17,52),xlabel='Coupe locale, mm (axe illustratif)',ylabel='Z mm',title='USER : chaîne cotée et arrêt de surcharge de doigt — pas un poussoir fonctionnel qualifié')
ax.legend(loc='lower right',fontsize=8);ax.grid(alpha=.2);fig.tight_layout()
fig,(ax,bx)=plt.subplots(1,2,figsize=(11.7,8.3));figs.append(fig)
for name,lo,hi,color in [('Cadre vitre',[24,-16],[50,2],'blue'),('Insert ST-LINK V1.5',[52,-20],[84,4],'orange'),('Insert nappe V1.5',[41,4],[66,25],'purple')]:
 ax.add_patch(Rectangle(lo,hi[0]-lo[0],hi[1]-lo[1],fill=False,color=color,lw=2,label=name))
ax.add_patch(Rectangle((29,-11),16,8,facecolor='cyan',alpha=.4,label='Vue utile 16 × 8'))
ax.text(24,29,'Deux intervalles nominaux de2mm\navec les inserts V1.5 ; fit câbles inconnu.')
ax.set(xlim=(18,90),ylim=(-24,35),xlabel='X KiCad mm',ylabel='Z mm',title='Face ST-LINK : fenêtre fermée proposée')
ax.set_aspect('equal');ax.legend(loc='lower right',fontsize=8);ax.grid(alpha=.2)
bx.add_patch(Rectangle((55,-12),26,17,fill=False,color='orange',label='Réservation ST-LINK'))
bx.plot([29,45],[-20,-20],color='blue',lw=4,label='Vitre latérale')
for row in ok[::max(1,len(ok)//6)][:6]:
 a=np.array(row['source_design_domain_xyz']);e=np.array(row['eye_xyz']);q=a+(e-a)*(-20-a[1])/(e[1]-a[1]);bx.plot([a[0],q[0]],[a[1],q[1]],color='green',ls='--')
bx.add_patch(Rectangle((45,12),20,16,fill=False,color='red',label='Domaine test, pas les LED'))
bx.set(xlim=(15,88),ylim=(35,-25),xlabel='X KiCad mm',ylabel='Y KiCad mm',title='Passage en plan : quelques rayons de test libres')
bx.set_aspect('equal');bx.legend(loc='lower right',fontsize=8);bx.grid(alpha=.2);fig.tight_layout()
with PdfPages(O/'v17-vues-coupes.pdf',metadata={'Title':'V1.7 study','CreationDate':None,'ModDate':None}) as pdf:
 for i,fig in enumerate(figs):
  pdf.savefig(fig);fig.savefig(O/f'vue-{i+1}.svg');plt.close(fig)
for f in O.glob('*.svg'):
 text=f.read_text();norm='\n'.join(l.rstrip() for l in text.splitlines())+'\n';assert text.split()==norm.split();f.write_text(norm)
print(json.dumps({'rays':len(results),'clear_conditional':len(ok),'obstructions':failures,'USER':budget,'STEP':step_read},ensure_ascii=False))
