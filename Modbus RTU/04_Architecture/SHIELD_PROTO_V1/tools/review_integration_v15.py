"""Independent scope/access checks and a two-page future physical checklist.
Does not communicate with hardware, and does not infer C05 centres from pictures.
"""
from pathlib import Path
import json, subprocess, hashlib, csv, sys
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from matplotlib.backends.backend_pdf import PdfPages
P=Path(__file__).resolve().parents[1];V=P/'mechanical/V1_5';Q=P/'docs/checks/v15';O=V/'REVIEW_ONLY'
C=json.loads((V/'parameters.json').read_text());D=json.loads((P/'design.json').read_text());B=json.loads((P/'docs/pcb_geometry.json').read_text())
BASE='10c6e0f22cd09f203ab79aedd79e7e5ebda3637f'
R=P.parents[2]
# Hardware/CAM exact preservation is tested against the mission start, not a hardcoded expected size.
protected=[p for folder in ['cad','exports/BLOCKED_FABRICATION']for p in (P/folder).rglob('*')if p.is_file()]+[P/'design.json',P/'docs/pcb_geometry.json',P/'exports/shield-nu.step',P/'cad/routes.json']
rows=[]
for p in sorted(set(protected)):
 rel=p.relative_to(R).as_posix();old=subprocess.check_output(['git','show',BASE+':'+rel],cwd=R);b=p.read_bytes();assert old==b,rel
 rows.append({'path':rel,'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest()})
changed=subprocess.check_output(['git','diff','--name-only','-z',BASE],cwd=R).decode().split('\0');assert not any(f.startswith(('experiments/','Modbus RTU/05_Firmware/','Supervision TR2/'))for f in changed if f)
(Q/'protected-PCB-CAM.json').write_text(json.dumps({'status':'PASS','base':BASE,'checked_files':len(rows),'files':rows,'ERC_DRC_V14_reused':'PCB/schematic/project/CAM byte-identical; new electrical run not justified','hardware_access':'NONE'},indent=2)+'\n')
# Separate BRep clearance checks on modelled objects. Nothing claims complete real-card geometry.
sys.path.insert(0,str(P/'tools'));import build_integration_v15 as M
parts={p['name']:p for p in M.parts};base=parts['Case_base_PETG_CONCEPT']['shape'];lid=parts['Case_lid_PETG_CONCEPT']['shape'];results=[]
for p in M.parts:
 if p['group']=='electronics':
  vol=base.intersect(p['shape']).Volume()+lid.intersect(p['shape']).Volume()
  results.append({'object':p['name'],'modeled_case_overlap_mm3':vol});assert vol<1e-5,p['name']
# Doors must be removed for service; closed doors intentionally obstruct access.
sweep=M.box(-10,88.3,12,21.1,103.5,15)
fixed=[p for p in M.parts if p['group']in ('case','structure')and not p['name'].startswith('Insert_microSD')]
sd_overlaps=[]
for p in fixed:
 vol=sweep.intersect(p['shape']).Volume()
 if vol>1e-5:sd_overlaps.append({'object':p['name'],'volume':vol})
assert not sd_overlaps,sd_overlaps
# Tool corridor above the nominal terminal tops, after unscrewing only its local cap.
tool=M.box(132,80,21.1,142,104,45)
rs_overlaps=[]
for p in M.parts:
 if p['group']=='case'and p['name']!='RS485_tool_cover':
  vol=tool.intersect(p['shape']).Volume()
  if vol>1e-5:rs_overlaps.append({'object':p['name'],'volume':vol})
assert not rs_overlaps,rs_overlaps
r={'status':'PASS_BOUNDED_NOMINAL_MODEL_CHECKS','electronics_vs_base_lid':results,'SD_card_rectangular_sweep':'no modelled fixed case/structure intersection with SD door removed; does not test finger ergonomics or actual ejection','RS485_tool_corridor':'no modelled case intersection with local tool cap removed; actual terminal type/driver still unverified','LED_north_sector':'intersects shield projection; requires actual C05 positions before deciding PCB passage','USER_south_sector':'not a measured button swept volume; no guarantee of shield clearance','all_actual_C05_fit':'NOT_VERIFIED'}
(Q/'access-clearance-checks.json').write_text(json.dumps(r,ensure_ascii=False,indent=2)+'\n')
# Every requested interface has an explicit verdict, including the real unknowns.
matrix=[
 ('J5/Zio','BLOCKED','G real inconnu; exemple G11: jeu0,09mm, insertion5,73mm','G naturel + profil engagement et tolérances; pas de nouvelle cote8,22/2,40'),
 ('LD1 verte PC7','BLOCKED','Secteur nord; centre/hauteur inconnus, shield/MKI potentiellement au-dessus','Centre/hauteur + trajet optique 3D; ne pas percer sur secteur approximatif'),
 ('LD2 bleue PB7','BLOCKED','Même secteur; pas de guide effectivement couplé','Même photo cotée que LD1'),
 ('LD3 rouge PG2','BLOCKED','Même secteur; ne pas confondre avec LD4 ST-LINK','Même photo cotée que LD1'),
 ('USER B1 PC13','BLOCKED','Principe captif démontré sur coupon seulement; centre/course/force inconnus','XY et hauteur B1/capuchon, course/datasheet; appui au repos interdit'),
 ('RESET B2','RESERVE','Pas de trou externe; panneau USER reste aveugle','Éloignement réel du futur actionneur à contrôler sur photo'),
 ('CN15 USB-C 5V','RESERVE','Ouverture et insert démontable modélisés au sud; centre réel inconnu','Position prise + enveloppe fiche/câble existant, jeu et retenue'),
 ('CN1 ST-LINK','RESERVE','Réservation nord séparée en Z de la nappe; pas une découpe cotée C05','Photo profil prise + fiche maintenance réelle'),
 ('microSD','PASS','Géométrie fabricant alignée 9pads/2trous; balayage carte libre du boîtier sans trappe','PASS nominal seulement; poussée/doigts et carte réelle restent RESERVE'),
 ('RS485 fils/outil','PASS','Couloir outil au-dessus bornier v101 et capot distinct; passage +X libre nominal','PASS nominal seulement; version bornier, outil/fils/gland réels RESERVE'),
 ('Nappe 12voies30cm','BLOCKED','Couloir réservé, pas de raccordement inventé à un IDC MKI inconnu','Photo connecteur/orientation/hauteur; rayon et fixation de nappe'),
 ('MKIGIBV2','BLOCKED','Pitch15,24 nominal/15,2 acquis; corps/IDC restent approximatifs','Enveloppe/profil et pose étrier, sans redemander entraxe'),
 ('Click / isolement','RESERVE','Modèle v101; aucun ajout conducteur GND2-GND logique; structure plastique côté logique','Révision réelle + hauteur/composants/appui étrier; ne pas rapprocher masse isolée'),
 ('Trous/ancrages','RESERVE','Centres PCB inchangés, trois supports et étriers représentés','Gabarit100% sur deux C05; OD4 entrecartes et têtes<=4,5 horsPCB'),
 ('Fond/couvercle/vis','PASS','Solides valides, STL fermés; modules modélisés sans collision avec fond/couvercle','PASS géométrique nominal; tolérances/PETG/température/supports impression RESERVE')]
with (P/'docs/MATRICE_INTERFERENCES_V1_5.csv').open('w')as f:
 w=csv.writer(f,lineterminator="\n");w.writerow(['interface','verdict','preuve_limitee','reste_a_faire']);w.writerows(matrix)
# Print-ready 2-page checklist. The regions are intentionally not false exact component centres.
with PdfPages(O/'CONTROLE_PHYSIQUE_MINIMAL_2P.pdf')as pdf:
 fig,ax=plt.subplots(figsize=(8.27,11.69));fig.subplots_adjust(top=.90,bottom=.27,left=.12,right=.93)
 ax.set_aspect('equal');ax.set_xlim(10,145);ax.set_ylim(143,-24);ax.set_xlabel('X KiCad / mm');ax.set_ylabel('Y KiCad / mm')
 ax.add_patch(Rectangle((21.75,-6.75),70,133.34,fill=False,ec='#25597b',lw=2));ax.text(94,5,'Nucleo:\nreconstruction UM\nnon certifiée C05',fontsize=8,color='#25597b')
 ax.add_patch(Rectangle((20,17),90,100,fill=False,ec='#177c35',lw=2));ax.text(95,114,'Shield\n90x100',fontsize=8,color='#177c35')
 for name,coords in C['control_regions'].items():
  if not name.endswith('_xy'):continue
  (x,y),(xx,yy)=coords;ax.add_patch(Rectangle((x,y),xx-x,yy-y,fill=False,ec='#b21d28',linestyle='--'));ax.text(xx+1,(y+yy)/2,name.split('_')[0],fontsize=8,color='#b21d28')
 for name,b in [('MKI (corps hypothétique)',(42.46,21.46,20.32,33.02)),('SD (fabricant)',(20.18,83.14,22.86,25.4)),('Click v101',(83.41,78.73,57.15,25.4))]:
  x,y,w,h=b;ax.add_patch(Rectangle((x,y),w,h,fill=False,ec='#bb8a25'));ax.text(x+1,y+3,name,fontsize=6.5)
 o=next(q for q in B['pads']if q['ref']=='J2'and q['number']=='4');ax.plot(o['x'],o['y'],'ko');ax.annotate('O = CN8-4\norigine de relevé',xy=(o['x'],o['y']),xytext=(12,63),arrowprops={'arrowstyle':'->'},fontsize=8)
 for x,y in D['mounting_holes_mm']:ax.plot(x,y,'o',mfc='none',mec='black',ms=5)
 fig.suptitle('TR2 V1.5 — contrôle futur, 1/2\nSecteurs à inspecter, aucun centre LED/USER inventé',fontsize=13)
 fig.text(.08,.13,'1. Bancs hors tension, sans démontage/câblage sous cette mission.\n2. Une photo dessus de chaque C05, orthogonale avec règle dans le plan PCB.\n   Repérer CN8-4, LD1/2/3, B1 USER et B2 RESET.\n3. Relever les centres par rapport à O (axes du plan), puis hauteurs au PCB.\n   LED: surface émissive; USER: capuchon en place, pas le seul corps du switch.\n4. Comparer le gabarit PCB V1_4 à100% sur les deux cartes.\n   Ne pas déduire un ajustement de cette page: ce plan n’est pas le gabarit de perçage.',fontsize=9)
 pdf.savefig(fig);plt.close(fig)
 fig=plt.figure(figsize=(8.27,11.69));fig.suptitle('TR2 V1.5 — contrôle futur, 2/2\nUne série groupée de mesures utiles, pas de nouvelle campagne',fontsize=13,y=.95)
 ax=fig.add_axes([.12,.65,.80,.21]);ax.set_xlim(0,100);ax.set_ylim(-24,42);ax.set_xlabel('Profil schématique (pas une cote C05)');ax.set_ylabel('Z / dessus shield')
 for zz,lab,col in [(-14.2,'dessous Nucleo (hypothèse)','#25597b'),(-12.6,'dessus Nucleo (hypothèse)','#25597b'),(-1.6,'dessous shield','#177c35'),(0,'dessus shield','#177c35'),(37,'couvercle interne (proposé)','#555')]:
  ax.plot([5,95],[zz,zz],color=col);ax.text(7,zz+1.2,lab,fontsize=7)
 ax.annotate('',xy=(91,-1.6),xytext=(91,-12.6),arrowprops={'arrowstyle':'<->'});ax.text(92,-8,'G réel\nà mesurer',fontsize=7)
 text='''A — Une vue de profil avec échelle: G naturel Zio quand connecteurs témoins disponibles,\n      aucune force ni serrage pour compenser. Confirmer corps/queues et composants.\n      G exemple11,00 => jeu J5 seulement0,09; ce n’est pas une cote acceptée.\nB — Même série: hauteurs émissives LD1/2/3 et B1 USER. Si course B1 inconnue,\n      identifier sa référence ou relever hors tension repos/appui: pas de force arbitraire.\n      Le poussoir ne sera coupé/réglé qu’avec jeu, butée et effort admissible connus.\nC — Une photo dessus/profil MKI et Click, connecteurs et révision lisibles.\n      Relever seulement enveloppes/hauteurs qui dépassent les réservations du plan.\n      Contrôler les appuis d’étriers, l’IDC et les zones de composants/isolement.\nD — Photos de CN15 et ST-LINK avec fiches réellement prévues, dimensions hors-tout\n      de leur surmoulage; dégagement pour insertion/retrait et rayon de câble.\n      Vérifier extraction SD/poussée/doigts par la trappe et outil du bornier.\nE — Nappe d’origine: fiche, sens de départ et hauteur. Prévoir serre-nappe sur boîtier,\n      jamais sur capteur/Click. Pas de contact avec GND2 ni pont métallique d’isolement.\n\nAcquis À NE PAS REFAIRE: CN12 mâles66/67/68/69, pointe8,22 et plastique2,40mm;\nentraxe MKI15,2mm et GND déjà identifiés. Rien à flasher ni à écrire sur SD/FRAM.\n\nCes actions sont une liste pour une intervention ultérieure autorisée.\nAucune manipulation n’est demandée maintenant. Si un connecteur témoin manque,\nne pas acheter ou démonter un banc: signaler l’indisponibilité. Après retour des cotes,\nplacer réellement LED/B1/USB, tester les trajets et décider toute découpe AVANT PCB.'''
 fig.text(.08,.11,text,fontsize=9,linespacing=1.5);pdf.savefig(fig);plt.close(fig)
print('PASS: PCB/CAM protection',len(rows),'files; bounded access checks; two-page physical checklist')
