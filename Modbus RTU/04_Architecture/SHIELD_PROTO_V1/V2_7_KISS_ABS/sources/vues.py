"""Numbered full exploded view, closed/open prototype, true CAD section and assembly PDF."""
from pathlib import Path
import json,math
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from matplotlib.collections import PolyCollection
import assemblage as a
v=a.v;m=v.m;P=v.P
CACHE={}
def triangles(s):
 key=s.hashCode()
 if key not in CACHE:
  vs,fs=s.tessellate(.13,.25);xyz=np.array([[p.x,p.y,p.z]for p in vs]);CACHE[key]=xyz[np.array(fs)]
 return CACHE[key]
def draw(ax,s,col,alpha,offset=(0,0,0)):
 tr=triangles(s)+np.array(offset);norm=np.cross(tr[:,1]-tr[:,0],tr[:,2]-tr[:,0]);lens=np.linalg.norm(norm,axis=1);norm/=np.where(lens>0,lens,1)[:,None]
 light=np.array([.25,-.4,.88]);shade=.5+.45*np.abs(norm@light);colors=np.clip(np.array(col)[None,:]*shade[:,None],0,1)
 ax.add_collection3d(Poly3DCollection(tr,facecolors=colors,edgecolors='none',alpha=alpha,rasterized=True))
def render(gg,mode,path):
 from rendu_profondeur import render as depth_render
 return depth_render(gg,mode,path,triangles,a.explode_offset)

def page_text(pdf,title,lines):
 f,ax=plt.subplots(figsize=(8.27,11.69));ax.axis('off');f.suptitle(title,fontsize=15,fontweight='bold',y=.95);ax.text(0,.93,lines,va='top',fontsize=10,linespacing=1.5,wrap=True);f.text(.1,.025,'V2.7 • deux prototypes • ABS • contrôle à froid avant alimentation',fontsize=8,color='#55606d');pdf.savefig(f,bbox_inches='tight');plt.close(f)
def section():
 for n in ['02_couvercle_ARNACO_USER','40_cage_USER_captive','41_USER_pointe_Z10p4']:
  if n not in v.PARTS:v.PARTS[n]={'shape':v.cq.importers.importStep(str(P/'cad'/(n+'.step'))).val()}
 f,ax=plt.subplots(figsize=(11.69,8.27));f.suptitle('USER — coupe réelle sur l’axe et la butée',fontsize=15,fontweight='bold')
 slab=m.box(v.UX-.05,110,0,v.UX+.05,138,54)
 shapes=[('capot',v.PARTS['02_couvercle_ARNACO_USER']['shape'],a.COL['case']),('cage',v.PARTS['40_cage_USER_captive']['shape'],a.COL['USER']),('tige',v.PARTS['41_USER_pointe_Z10p4']['shape'],a.COL['Click'])]
 x,y=v.STOP;shapes +=[('butée',a.c.bolt(x,y,32.8,12,1.5,3,3.2),a.COL['metal']),('écroufrein',a.c.nut(x,y,34.6,5.5,4,1.5),a.COL['metal']),('ressort',m.cyl(v.UX,v.UY,36,3.525,9).cut(m.cyl(v.UX,v.UY,35.9,2.75,9.2)),(.22,.65,.57)),('B1enveloppe',m.box(v.UX-2,v.UY-2,5,v.UX+2,v.UY+2,10.2),(.22,.55,.86))]
 for n,s,col in shapes:
  cut=s.intersect(slab)
  if abs(cut.Volume())<1e-7:continue
  tr=triangles(cut);yz=np.stack([-tr[:,:,1],tr[:,:,2]],axis=2);ax.add_collection(PolyCollection(yz,facecolors=[col],edgecolors='none',alpha=.85))
 ax.set(xlim=(111,137),ylim=(3,53),xlabel='Y KiCad (mm)',ylabel='Z depuis dessus Nucleo (mm)');ax.set_aspect('equal');ax.grid(alpha=.15)
 for xy,txt,pt in [((v.UY,49.4),'Tête en retrait 0,6 mm',(112,51)),((v.UY,46),'Collerette captive Ø23',(111,46)),((v.UY,36),'Guidage Ø4,8 sur 18 mm',(111,32)),((v.UY+9,44.8),'Butée M3 réglable ; réglage court 0,2',(130,49)),((v.UY,40),'Ressort courant captif',(130,39)),((v.UY,10.4),'Pointe à ajuster ; B1 haut10,2 rapporté',(130,13))]:ax.annotate(txt,xy,pt,fontsize=8,arrowprops={'arrowstyle':'->'},ha='left',va='center')
 f.text(.04,.025,'Attaches latérales hors coupe. Réglage à froid obligatoire : jeu au repos, premier clic puis butée, retour franc.\nLe réglage court montré n’est pas un départ opérationnel. Course/force réelles de B1 et retrait ABS non qualifiés.',fontsize=9);f.savefig(P/'vues/COUPE_USER.png',dpi=180,bbox_inches='tight');return f

def main():
 gg,prov=a.groups();render(gg,'exploded',P/'vues/VUE_ECLATEE_V27.png');render(gg,'closed',P/'vues/BOITIER_FERME.png');render(gg,'open',P/'vues/IMPLANTATION.png');sec=section()
 document(sec)

def document(sec=None):
 if sec is None:sec=section()
 rows=json.loads((P/'preuves/pieces.json').read_text())
 with PdfPages(P/'vues/PLAN_MONTAGE_V27.pdf')as pdf:
  f,ax=plt.subplots(figsize=(8.27,11.69));ax.imshow(plt.imread(P/'vues/VUE_ECLATEE_V27.png'));ax.axis('off');f.subplots_adjust(0,0,1,1);pdf.savefig(f);plt.close(f)
  f,axs=plt.subplots(2,1,figsize=(8.27,11.69));f.suptitle('Assemblage nominal — pas une photographie',fontsize=14,fontweight='bold')
  for ax,n in zip(axs,['BOITIER_FERME.png','IMPLANTATION.png']):ax.imshow(plt.imread(P/'vues'/n));ax.axis('off')
  f.tight_layout();pdf.savefig(f);plt.close(f);pdf.savefig(sec,bbox_inches='tight');plt.close(sec)
  f,ax=plt.subplots(figsize=(8.27,11.69));ax.axis('off');f.suptitle('Morvan : 25 STL, 61 pièces pour deux TR2',fontsize=14,fontweight='bold');data=[[q['id'],q['quantity_to_print_two_kits'],'variante'if q['id'].startswith(('20_','41_'))else'coupon'if q['id'].startswith('50_')else'6 utiles +2 réserve'if q['id']=='13_D126'else'utile']for q in rows]
  t=ax.table(cellText=data,colLabels=['Nom de fichier (sans .stl)','Qté','Usage'],loc='center',colWidths=[.61,.09,.3]);t.auto_set_font_size(False);t.set_fontsize(8);t.scale(1,1.65)
  f.text(.10,.055,'Montage utile : 46 pièces (23 par TR2). Variantes : 12. D126 réserve : 2. Coupon : 1.\nLes familles H10,6 /11,2 /11,8 et tiges USER sont marquées de 1 /2 /3 petits creux.\nTout imprimer une fois selon les quantités ; ne monter qu’une hauteur par module.',fontsize=9);pdf.savefig(f);plt.close(f)
  page_text(pdf,'Montage hors tension — ordre conseillé','''1. Charger les neuf écrous captifs de la caisse nue. Ébavurer les pièces loin des cartes.

2. Préinsérer les huit M2×30 et leurs rondelles isolantes sous le shield,
   avant l’empilage Nucleo / D126 / shield. Maintenir provisoirement les vis.
   Serrer les Ajile avant de poser la microSD et les étriers qui en masquent l’accès.
   Retirer tout maintien provisoire ; ne laisser ni ruban, copeau ou écrou libre.

3. Emboîter FRAM / microSD à la main jusqu’à leur butée naturelle.
   Choisir UNE selle par module. Poncer à plat les deux portées, au plus 0,6 mm,
   sans fléchir les cartes ni forcer les prises. Ajouter les rondelles supérieures
   puis les deux écrous M2 en contre-écrou. Aucun calage imprimé séparé.

4. Monter les étriers et patins : uniquement sur PCB nu ; métal hors modules.
   Confirmer les portées du Click réel1.02 (STEP source1.01). Régler doucement.

5. Installer la façade nord avant de coucher la nappe dans son U supérieur ;
   l’IDC ne traverse pas la fente. Relier RS-485 capot déposé, poser le câble
   sur la languette, puis descendre le panneau est autour du câble déjà branché.
   Colson et garniture aux deux brides. La SD reste accessible par sa poche fixe.

6. Préassembler la cage USER au capot avec ressort, tige et vis de butée freinée.
   Régler hors tension sur le vrai B1 : aucune précharge, premier clic atteint
   puis butée immédiate, retour franc. En service : une pression du doigt,
   sans ouvrir une trappe. RESET n’a pas de commande alignée.

Contrôle final : G12,25±0,10 ; tête M2 dégagée ; sockets non forcés ;
PCB non fléchis ; SD / CN15 / ST-LINK / nappe / bornier libres ; trois LED visibles.
Le capot entier donne accès au bornier en maintenance ; aucune trappe supérieure.
Force/course B1, gants, vibrations, fluage, retrait machine et IP : non qualifiés.''')
  page_text(pdf,'ABS et fournitures — aucun fichier à redessiner','''ABS non conducteur, une couleur. STL orientés en millimètres, échelle100%.
Vérifier sur le vrai plateau : base / capot158×166 mm, plus bordure d’adhésion.
Aucun plateau ni temps d’impression garanti. Profil ABS du filament et de la
machine, enceinte adaptée ; pas de correction de retrait globale improvisée.

Coupon50 : alésages2,4 /3,6 /4,8 /13 ; écrou AF6,1 ; témoins12,25 et11,2.
D126 : H12,15..12,35 après ébavurage, couches0,10, 100%.
Autres : couches0,15..0,20, quatre périmètres, 25–35% dans les grands volumes.
Selles / cage / patins / poussoirs : 100%, couches0,10..0,15.
Supports locaux sous la collerette des poussoirs et si nécessaire dans les poches,
à retirer entièrement. Aucun filetage imprimé. Morvan n’a pas à refaire la CAO.

Quincaillerie utile pour les DEUX TR2, hors pièces déjà acquises :
• 40 M3×12 métal ;30 écrous M3 DIN934 ;10 écrous frein M3 DIN985.
• 16 M2×30 métal ;32 écrous M2 ordinaires.
• 24 rondelles isolantes nylon DIN433 M2 :2,2×4,5×0,3.
  16 sous les têtes (8 par TR2), 8 au-dessus des modules Adafruit.
• 2 ressorts de rappel : exemple courant D-064 DIN2098,
  fil0,5 /extérieur6,8 /libre13,5 mm ; vérifier les équivalents avant montage.
• 4 Colson, garnitures souples, 2 vitres50×18×1,5.
Ajile acquis :6 M2×20 nylon +12écrous utiles uniquement D126 ; stock8/16.

Le STEP éclaté est un document d’assemblage, PAS une pièce à imprimer.
Seuls les STL listés dans QUANTITES.csv sont à produire.
Les couleurs du plan ne demandent aucun filament supplémentaire.
Avant transmission de ce dossier : revue indépendante ChatGPT.
Aucune qualification physique ou industrielle déduite de la CAO.''')
 print('Exploded PNG + closed/open PNG + actual section; 6-page PDF generated',flush=True)
if __name__=='__main__':
 import sys
 document()if '--pdf-only'in sys.argv else main()
