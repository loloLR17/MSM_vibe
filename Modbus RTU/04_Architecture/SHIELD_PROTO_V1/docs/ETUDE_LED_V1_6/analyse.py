"""Etude sans modification CAO. Executer avec Python + numpy + matplotlib.
Les volumes LED sont des domaines de conception hypothetique, jamais des mesures C05.
"""
from pathlib import Path
import hashlib, json, subprocess, math
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle, Circle
from matplotlib.backends.backend_pdf import PdfPages

HERE = Path(__file__).resolve().parent
ROOT = Path(subprocess.check_output(['git', 'rev-parse', '--show-toplevel'], cwd=HERE, text=True).strip())
SHIELD = HERE.parent.parent
BASE = '6d97906d32ec6b30a6db9caa140fc02481aa142d'
OUT = HERE / 'preuves'

def write(name, obj):
    (OUT/name).write_text(json.dumps(obj, ensure_ascii=False, indent=2)+'\n')

def git(*args):
    return subprocess.check_output(['git', *args], cwd=ROOT)

def freeze():
    entries=[]
    for raw in git('ls-tree', '-rz', BASE, '--', str(SHIELD.relative_to(ROOT))).split(b'\0'):
        if not raw: continue
        meta, path = raw.split(b'\t'); mode, kind, blob = meta.split()
        assert kind == b'blob'
        name=path.decode(); local=(ROOT/name).read_bytes()
        original=git('cat-file', 'blob', blob.decode())
        assert local == original, name
        entries.append({'path':name,'bytes':len(local),'sha256':hashlib.sha256(local).hexdigest()})
    # Toute modification suivie doit etre documentaire et explicitement dans le perimetre.
    changed=git('diff','--name-only',BASE).decode().splitlines()
    allowed=['Modbus RTU/00_gouvernance/ETAT_COURANT_TR2.md',
             'Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md']
    assert all(n in allowed or '/ETUDE_LED_V1_6/' in n or
               '/archives/TR2-20261104-SHIELD-V16-ETUDE-LED/' in n for n in changed), changed
    write('conservation.json', {'base':BASE,'comparison':'all pre-existing shield tracked bytes equal initial Git blobs',
                              'count':len(entries),'files':entries,'tracked_diff_scope':changed,
                              'firmware_modified':False,'manufacturing_files_modified':False})

native=json.loads((OUT/'pcb_natif_lecture_seule.json').read_text())
assert len(native['pads'])==191 and len(native['tracks'])==396 and native['zones']==2
# Rectangle de scenario d'ouverture : PAS un detourage ni une empreinte de LED.
rect=[45,17,65,28]

def intersect_segment_box(a,b,lo,hi):
    a=np.array(a,float); b=np.array(b,float); lo=np.array(lo,float); hi=np.array(hi,float)
    t0,t1=0.,1.
    for i,d in enumerate(b-a):
        if abs(d)<1e-12:
            if a[i]<lo[i] or a[i]>hi[i]: return False
        else:
            u,v=sorted(((lo[i]-a[i])/d,(hi[i]-a[i])/d))
            t0=max(t0,u);t1=min(t1,v)
            if t0>t1:return False
    return True

pads=[]; tracks=[]
for a in native['pads']:
    x,y=a['xy']; r=a['diameter_mm']/2
    if x+r>=rect[0] and x-r<=rect[2] and y+r>=rect[1] and y-r<=rect[3]:pads.append(a)
for t in native['tracks']:
    r=t['width_mm']/2
    if intersect_segment_box(t['start'],t['end'],[rect[0]-r,rect[1]-r],[rect[2]+r,rect[3]+r]):tracks.append(t)
write('impact_ouverture_hypothetique.json', {'rectangle_xy_mm':rect,'status':'WHAT_IF_NOT_C05_NOT_A_CUT',
    'pad_intersections_bbox':pads,'track_intersections_inflated_rectangle':tracks,
    'nets':sorted(set(t['net'] for t in tracks)),
    'zones':native['zones'],'zone_note':'GND zones present: cut would require refill and isolation review',
    'MKI_provisional_front_Y':21.46,'edge_to_MKI_mm':21.46-17,
    'guide_hole_mm':3.0734,'strip_centre_Y_with_half_mm_margins':[17+.5+3.0734/2,21.46-.5-3.0734/2]})

# XYZ = X/Y KiCad, Z vers le haut depuis le dessus du shield (Y CAD = -Y ici).
eye=np.array([220,-80,150.])
domain=[[45,18,-12.6],[65,21,-9.6]]
aperture=[[95,-16],[118,-6]]
boxes={
 'shield':[[20,17,-1.6],[110,117,0]],
 'MKI_provisional':[[42.46,21.46,11],[62.78,54.48,21]],
 'ribbon_reservation':[[46,-14,9],[61,21,22]],
 'MKI_post_1_bbox':[[36,17,0],[42,23,25]],
 'MKI_post_2_bbox':[[68,51.5,0],[74,57.5,25]],
 'MKI_roof_bbox':[[36,17,22],[74,57.5,25]],
 'STLINK_reservation_NOT_measured':[[55,-12,-17],[81,5,1]],
 'J8_conservative_body':[[43.5,23,0],[61.5,56,11]],
}
margin=.5
rays=[];angles=[];passes=[]
for x in np.linspace(domain[0][0],domain[1][0],11):
 for y in np.linspace(domain[0][1],domain[1][1],16):
  for z in np.linspace(domain[0][2],domain[1][2],7):
   a=np.array([x,y,z])
   assert not any(intersect_segment_box(a,eye,np.array(lo)-margin,np.array(hi)+margin) for lo,hi in boxes.values()), a
   for roof in [37,40]:
    q=a+(eye-a)*(roof-z)/(eye[2]-z)
    assert np.all(q[:2]>=aperture[0]) and np.all(q[:2]<=aperture[1]),q
    passes.append(q.tolist())
   angles.append(math.degrees(math.atan2(np.linalg.norm((eye-a)[:2]),eye[2]-z)))
   rays.append(a.tolist())
qmin=np.min(passes,axis=0);qmax=np.max(passes,axis=0)
# Exact monotonic extremum at expanded shield bottom for this rectangular domain.
worst_y=21+(-80-21)*(-2.1+9.6)/(150+9.6)
assert worst_y < 17-margin
gap=(17-margin)-worst_y
optical={'status':'CONDITIONAL_GEOMETRY_ONLY_NOT_PHOTOMETRY_OR_C05',
 'actual_LD1_XYZ':None,'actual_LD2_XYZ':None,'actual_LD3_XYZ':None,
 'domain_is_design_hypothesis_not_tolerance':domain,'common_eye_xyz':eye.tolist(),
 'aperture_xy_mm':aperture,'lid_z_mm':[37,40],'proposed_clear_plate_mm':1.5,
 'tested_obstacle_boxes':boxes,'expanded_obstacles_margin_mm':margin,'grid_rays':len(rays),
 'all_grid_rays_clear':True,'window_crossing_min':qmin.tolist(),'window_crossing_max':qmax.tolist(),
 'view_angle_from_vertical_deg':[min(angles),max(angles)],
 'analytical_worst_ray_Y_at_expanded_shield_bottom':worst_y,
 'clearance_to_expanded_shield_north_edge_mm':gap,
 'unknowns':'actual LED emitting area/height/orientation/brightness, C05 north components, natural G, actual ribbon and access to eye position',
 'not_exhaustive_CAD_collision_check':True,
 'vertical_gap_to_lid_inner_mm':[37-domain[1][2],37-domain[0][2]],
 'Bivar_longest_in_consulted_PLP1_family_mm':21.1}
write('optique_conditionnelle.json',optical)
electrical={
 'source':'C05 schematic sheets3/5 text; UM2861 Rev8 p24/p37',
 'active_high':True,'Nucleo_LED_supply':'3V3','no_direct_LED_parallel':True,
 'channels':[
 {'LED':'LD1','color':'green','GPIO':'PC7','Morpho':'CN12-19','Q':'Q3','Rb':'R49 47k','Rbe':'R45 100k','Rled':'R25 510','Vf_illustration':2.0,'R_ohm':510},
 {'LED':'LD2','color':'blue','GPIO':'PB7','Morpho':'CN11-21','Q':'Q4','Rb':'R50 47k','Rbe':'R46 100k','Rled':'R23 680','Vf_illustration':2.8,'R_ohm':680},
 {'LED':'LD3','color':'red','GPIO':'PG2','Morpho':'CN11-42','Q':'Q5','Rb':'R51 47k','Rbe':'R47 100k','Rled':'R24 1000','Vf_illustration':1.8,'R_ohm':1000}],
 'fallback':{'Rb_ohm':4700,'Rbe_ohm':100000,'Rled_candidate_ohm':1000,'tolerance':.05,
 'extra_gpio_upper_mA':3.6/(4700*.95)*1000,'factory_gpio_upper_mA':3.6/(47000*.95)*1000,
 'extra_LED_branch_upper_mA':3.6/950*1000,
 'rail_upper_three_LED_and_bases_mA':3*(3.6/950+3.6/(4700*.95))*1000,
 'Rled_power_upper_mW':3.6**2/950*1000,
 'parts_per_board':{'LED':3,'NPN':3,'R':9,'new_mating_signal_pairs':3},
 'status':'principle only, verify actual LED Vf/brightness, chosen BJT pinout and IO domain; not approved at 1.8V'},
 'current_examples_not_measurements':True}
for c in electrical['channels']:
 c['factory_current_example_mA']=(3.3-c['Vf_illustration']-.1)/c['R_ohm']*1000
 c['fallback_current_example_mA']=(3.3-c['Vf_illustration']-.1)/1000*1000
write('electrique.json',electrical)

plt.rcParams.update({'font.size':9,'svg.fonttype':'none'})
pages=[]
fig,ax=plt.subplots(figsize=(11.7,8.3));pages.append(fig)
ax.set_title('V1.6 — plan coté de scénario, sans découpe ni coordonnées LED C05')
ax.add_patch(Rectangle((20,17),90,100,fill=False,lw=2,label='PCB existant 90 × 100'))
for t in native['tracks']:
 a,b=t['start'],t['end'];ax.plot([a[0],b[0]],[a[1],b[1]],color='#b57365',lw=.5,alpha=.6)
for a in native['pads']:
 x,y=a['xy'];ax.add_patch(Circle((x,y),a['diameter_mm']/2,color='#8a8a8a',alpha=.5))
ax.add_patch(Rectangle((42.46,21.46),20.32,33.02,fill=False,color='orange',lw=2,label='MKI : enveloppe provisoire'))
ax.add_patch(Rectangle((46,-14),15,35,fill=False,color='purple',lw=2,label='Nappe : réservation'))
ax.add_patch(Rectangle((45,17),20,11,facecolor='red',alpha=.15,label='Ouverture WHAT-IF 20 × 11, non retenue'))
ax.add_patch(Rectangle((45,18),20,3,fill=False,lw=2,color='green',label='Domaine de test (pas des LED placées)'))
ax.add_patch(Rectangle((95,-16),23,10,fill=False,lw=2,color='blue',label='Fenêtre proposée 23 × 10'))
for a in [np.array(domain[0]), np.array(domain[1])]:
 q=a+(eye-a)*(40-a[2])/(eye[2]-a[2]);ax.plot([a[0],q[0]],[a[1],q[1]],color='green',ls='--')
ax.text(23,115,'USER et RESET au sud : inchangés ; aucun fichier boîtier modifié.')
ax.text(72,7,'Visée vers le nord-est\n(X croissant, Y décroissant)')
ax.set_xlim(10,130);ax.set_ylim(125,-22);ax.set_aspect('equal');ax.set_xlabel('X KiCad (mm)');ax.set_ylabel('Y KiCad (mm), nord en haut');ax.grid(alpha=.2)
leg=ax.legend(loc='upper left',bbox_to_anchor=(1.01,1),fontsize=8);leg.set_in_layout(False)
fig.tight_layout(rect=[0,0,.71,1]);fig.savefig(HERE/'01-plan.svg')

fig,ax=plt.subplots(figsize=(11.7,8.3));pages.append(fig)
ax.set_title('Coupe projetée Y/Z — famille de rayons conditionnels, G = 11 mm supposé')
ax.add_patch(Rectangle((17,-1.6),100,1.6,color='#96522c',label='Shield existant'))
ax.add_patch(Rectangle((-6.75,-14.2),133.34,1.6,color='#418059',label='Nucleo nominale'))
ax.add_patch(Rectangle((21.46,11),33.02,10,alpha=.4,color='orange',label='MKI provisoire'))
ax.add_patch(Rectangle((-14,9),35,13,fill=False,color='purple',ls='--',label='Projection nappe (évitée en X)'))
ax.plot([-20,146],[37,37],color='black',label='Sous-face couvercle z37')
for a in [np.array(domain[0]),np.array(domain[1]),np.array([45,21,-9.6])]:
 q=a+(eye-a)*(40-a[2])/(eye[2]-a[2]);ax.plot([a[1],q[1]],[a[2],q[2]],color='green')
ax.plot([-16,-6],[38.5,38.5],color='blue',lw=4,label='Fenêtre dans la projection, X95..118')
ax.annotate('Bord Y17 : passage devant le PCB\nclair au modèle seulement',xy=(17,-1.6),xytext=(36,-8),arrowprops={'arrowstyle':'->'})
ax.text(-18,46,'Plaque transparente 1,5 mm + cadre vissé + joint : principe fermé, aucune classe IP.')
ax.text(-18,51,f'{len(rays)} rayons testés ; angles {min(angles):.1f}–{max(angles):.1f}° de la normale LED supposée.')
ax.text(-18,56,'La projection Y/Z seule ne prouve pas le passage : contrôle XYZ et enveloppes dans le JSON.')
ax.text(-18,61,'Guides PLP1 étudiés ≤ 21,1 mm ; LED→sous-face toit 46,6–49,6 mm dans ce scénario.')
ax.set_xlim(-24,120);ax.set_ylim(-20,67);ax.set_aspect('equal');ax.set_xlabel('Y KiCad (mm)');ax.set_ylabel('Z (mm), dessus shield = 0');ax.grid(alpha=.2);ax.legend(loc='lower right',fontsize=8)
fig.tight_layout();fig.savefig(HERE/'02-coupe.svg')

fig,ax=plt.subplots(figsize=(11.7,8.3));pages.append(fig)
ax.set_xlim(0,160);ax.set_ylim(0,110);ax.axis('off')
ax.set_title('Repli électrique — schéma de principe × 3, pas un schéma KiCad de fabrication')
def wire(xs,ys):ax.plot(xs,ys,color='black',lw=1.6)
def box(x,y,w,h,label):
 ax.add_patch(Rectangle((x,y),w,h,facecolor='white',edgecolor='black'));ax.text(x+w/2,y+h/2,label,ha='center',va='center',fontsize=10)
wire([100,100,100],[96,85,80]);box(94,80,12,5,'1k*')
wire([100,100],[80,72]);box(94,65,12,7,'LED');wire([100,100],[65,58]);ax.text(104,67,'A en haut, K en bas')
ax.add_patch(Circle((100,50),8,fill=False));wire([95,95],[45,55]);wire([95,100],[53,58]);wire([95,100],[47,42]);ax.annotate('',xy=(100,42),xytext=(97,45),arrowprops={'arrowstyle':'->','color':'black'})
wire([100,100],[42,23]);ax.text(110,51,'Q NPN\nBC547B candidat\nB/C/E : vérifier la référence',fontsize=9)
wire([13,35],[50,50]);box(35,47,20,6,'4,7k');wire([55,95],[50,50]);wire([75,75],[50,39]);box(69,33,12,6,'100k');wire([75,75,100],[33,23,23]);wire([64,122],[23,23]);ax.text(124,23,'GND logique',va='center')
ax.text(100,99,'3V3 Nucleo',ha='center');ax.text(4,56,'GPIO via Morpho\n(conserve aussi\nle driver Nucleo)',fontsize=10)
ax.text(8,5,'Aucun lien vers GND2 isolée ; aucun changement firmware. HIGH → ON.\n*1k initial : choisir LED haute efficacité à faible Vf, puis qualifier luminosité / VDDIO2.')
rows=['Vert : PC7 → CN12-19 ; LD1 native Q3/R25=510Ω, R49=47k, R45=100k.',
      'Bleu : PB7 → CN11-21 ; LD2 native Q4/R23=680Ω, R50=47k, R46=100k.',
      'Rouge : PG2 → CN11-42 ; LD3 native Q5/R24=1k, R51=47k, R47=100k.']
for i,r in enumerate(rows):ax.text(5,108-i*5,r,fontsize=10)
ax.text(5,82,'CN7/8/9/10 : ces trois GPIO sont absents.\nJ5 actuel 65..70 ne les apporte pas.\nPA5/D13 est FRAM_SCK : ne pas l’utiliser.',fontsize=10)
fig.tight_layout();fig.savefig(HERE/'03-principe-electrique.svg')
with PdfPages(HERE/'ETUDE_LED_V1_6.pdf',metadata={'Title':'TR2 V1.6 — étude conditionnelle LED','CreationDate':None,'ModDate':None}) as pdf:
 for fig in pages:pdf.savefig(fig)
for fig in pages:plt.close(fig)
# Matplotlib SVG contient des blancs de fin de ligne ; normaliser le texte seulement.
for svg in HERE.glob('*.svg'):
    text=svg.read_text(); normalized='\n'.join(line.rstrip() for line in text.splitlines())+'\n'
    assert text.split()==normalized.split()
    svg.write_text(normalized)
freeze()
write('validation-calculs.json',{'status':'PASS_CONDITIONAL_MODEL','native_pads':len(native['pads']),
 'native_segments':len(native['tracks']),'what_if_pads':len(pads),'what_if_segments':len(tracks),
 'what_if_nets':sorted(set(t['net'] for t in tracks)),'rays':len(rays),'physical_validation':False})
print(json.dumps({'rays':len(rays),'angles':optical['view_angle_from_vertical_deg'],'window':optical['window_crossing_min'],
 'window_max':optical['window_crossing_max'],'edge_expanded_margin':gap,'cut_pads':[(a['ref'],a['number']) for a in pads],
 'cut_nets':sorted(set(t['net'] for t in tracks))},ensure_ascii=False))
