"""Own assembly/cut drawings; conservative supplier-derived boxes, no vendor solids exported."""
from pathlib import Path
import json,math
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from matplotlib.patches import Rectangle,Circle,Polygon
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import modele as v
P=v.P;m=v.m
BLUE='#266a9c';GREEN='#3d8767';ORANGE='#d7832d';GRAY='#6b7280'
def box(ax,x,y,w,h,color,alpha=.7,label=None):
 ax.add_patch(Rectangle((x,y),w,h,facecolor=color,edgecolor=color,alpha=alpha));
 if label:ax.text(x+w/2,y+h/2,label,ha='center',va='center',fontsize=8)
def title(f,s):f.suptitle(s,fontsize=14,fontweight='bold');f.text(.03,.018,'V2.6 • mm • numérique seulement • montage à froid obligatoire • PCB V2.3 et coffret V2.5 inchangés',fontsize=8,color=GRAY)
def finish(pdf,f,name=None):
 if name:f.savefig(P/'vues'/name,dpi=160,bbox_inches='tight')
 pdf.savefig(f,bbox_inches='tight');plt.close(f)
def main():
 v.build();rows=json.loads((P/'preuves/pieces.json').read_text())
 with PdfPages(P/'vues/ASSEMBLAGE_AJUSTEMENT_V26.pdf')as pdf:
  f,ax=plt.subplots(figsize=(8.27,11.69));title(f,'Kit complet pour deux TR2 — 25 types / 142 pièces');ax.axis('off')
  data=[[r['id'],str(r['quantity_to_print_two_kits']),'V2.5 identique'if r['inherited_exact_V25']else'Nouveau V2.6']for r in rows]
  t=ax.table(cellText=data,colLabels=['Nom du STL (sans extension)','À imprimer','Origine'],loc='center',colWidths=[.58,.16,.26]);t.auto_set_font_size(False);t.set_fontsize(9);t.scale(1,1.7)
  f.text(.1,.075,'Hauteurs 8/10/12 : choisir seulement 8 appuis sur les 24 imprimés.\n36 cales 1 mm = 24 rondelles fixes + 8 de calage + 4 de réserve.\n4 étriers inclus : 2 MKI + 2 Click. Patins : 8 utiles + 6 réserves.',fontsize=10)
  finish(pdf,f)
  f,ax=plt.subplots(figsize=(8.27,11.69));title(f,'Implantation réelle des retenues — vue du dessus')
  box(ax,20,17,90,100,'#d5e6d5',.65,'')
  for name,b,c in [('FRAM',[42.46,56.46,25.4,17.78],GREEN),('microSD',[20.18,83.14,22.86,25.4],BLUE),('MKI ST GKO',[43.612906,22.511306,17.998948,30.92704],ORANGE),('Click v101',[83.41,78.73,57.15,25.4],GRAY)]:
   box(ax,*b,c,.2);ax.text(b[0]+b[2]/2,b[1]+b[3]/2,name,ha='center',va='center',fontsize=8)
  for which,n,col in [('MKI','30_etrier_MKI',ORANGE),('Click','31_etrier_Click',BLUE)]:
   for a,b in v.CONFIG[which]['paths']:ax.plot([a[0],b[0]],[a[1],b[1]],color=col,lw=3)
   for x,y in v.CONFIG[which]['anchors']:ax.add_patch(Circle((x,y),2.8,fill=False,color=col));ax.text(x+3,y-2,'M2×30',fontsize=7,color=col)
   for(x,y),(tx,ty)in zip(v.CONFIG[which]['guides'],v.CONFIG[which]['contacts']):
    box(ax,x-4.2,y-4.2,8.4,8.4,col,.25);ax.plot([x,tx],[y,ty],color=col,lw=1.5);box(ax,tx-(1 if which=='MKI'else.4),ty-(.4 if which=='MKI'else 1),2 if which=='MKI'else.8,.8 if which=='MKI'else 2,col,1)
  for x,y in m.C['module_supports_FRAM']+m.C['module_supports_SD']:ax.add_patch(Circle((x,y),2.4,fill=False,color='black'));ax.plot(x,y,'k.',ms=3)
  ax.annotate('Sortie nappe côté ST-LINK\ncentrée X≈55,5, sans pli vif',(55.5,-15),(90,3),arrowprops={'arrowstyle':'->'},fontsize=9)
  ax.annotate('Carte SD : extraction vers −X',(21,96),(-1,118),arrowprops={'arrowstyle':'->'},fontsize=9)
  ax.annotate('Bornier : aucun pont métallique\nentre domaines isolés',(133,92),(105,126),arrowprops={'arrowstyle':'->'},fontsize=8)
  ax.set(xlim=(0,150),ylim=(138,-22),xlabel='X KiCad (mm)',ylabel='Y KiCad (mm)');ax.set_aspect('equal');ax.grid(alpha=.2)
  f.text(.1,.085,'Les contacts isolants sont de petits rectangles 0,8 × 2 mm.\nConfirmer les zones nues sur les vrais modules avant serrage.\nLes volumes des modules illustrés sont des contours dérivés, pas une photo.',fontsize=9)
  finish(pdf,f,'implantation.png')
  f,ax=plt.subplots(figsize=(11.69,8.27));title(f,'Coupe de principe FRAM / SD — chemin de compression rigide')
  box(ax,-16,0,32,1.6,GREEN,.6);ax.text(17,.8,'Shield (dessus Z13,85)',va='center')
  box(ax,-2.2,1.6,4.4,10,ORANGE,.7);box(ax,-2.2,11.6,4.4,1,'#e8b876');ax.text(4,6,'Appui 10 + cale 1\nexemple H=11, pas cote imposée')
  box(ax,-16,12.6,32,1.6,GREEN,.6);ax.text(17,13.4,'Module Adafruit (trou Ø2,5)',va='center')
  box(ax,-2.2,14.2,4.4,1,ORANGE);box(ax,-2,15.2,4,3.2,GRAY);ax.text(4,16.5,'Rondelle PETG 1 + deux écrous M2',va='center')
  box(ax,-2.2,-1,4.4,1,ORANGE);box(ax,-2.1,-3.2,4.2,2.2,GRAY);box(ax,-1,-1,2,30,GRAY,1);ax.text(4,-2,'Rondelle PETG 1 sous shield + M2×30',va='center')
  ax.annotate('',(-8,1.6),(-8,12.6),arrowprops={'arrowstyle':'<->'});ax.text(-11,7,'H',fontsize=14)
  ax.set(xlim=(-25,50),ylim=(-7,28));ax.set_aspect('equal');ax.axis('off')
  f.text(.1,.16,'1. Emboîter le module à sec à sa butée plastique, sans utiliser les vis.\n2. Choisir appui 8/10/12 + cales 0,5/1/2 ; poncer à plat au plus 0,5 mm, hors banc.\n3. Poser les deux appuis à la même hauteur. Zéro basculement, zéro flexion de la carte.\n4. Serrer légèrement les deux écrous en contre-écrou, sans transmettre le couple au module.\nPlage mécanique 8..15,5 mm ; ce n’est PAS une plage d’engagement électrique du connecteur.',fontsize=10)
  finish(pdf,f,'coupe-appui.png')
  f,ax=plt.subplots(figsize=(11.69,8.27));title(f,'Coupe d’un patin réglable — aucun métal sur le module')
  # Schematic X/Z section, not an exact manufacturing drawing.
  box(ax,-4.2,23.5,1.5,14.5,ORANGE);box(ax,2.7,23.5,1.5,14.5,ORANGE);box(ax,-4.2,36.8,8.4,1.2,ORANGE)
  box(ax,-2.65,32.8,5.3,4,GRAY);box(ax,-1.5,27.95,3,12,GRAY,1);box(ax,-3,39.95,6,3.2,GRAY,1)
  box(ax,-2.5,26.45,5,1.5,BLUE);box(ax,-2.5,27.95,1,1,BLUE);box(ax,1.5,27.95,1,1,BLUE);box(ax,2.5,27.45,4,1.5,BLUE);box(ax,5.5,26.45,.8,1.25,BLUE)
  box(ax,5.2,24.85,11,1.6,GREEN,.4)
  for yy,text in [(41.5,'M3×12, écrou frein DIN985 capturé'),(34.4,'Cage PETG : passage écrou avant montage'),(28.6,'Coupelle fermée : plancher PETG 1,5 mm'),(25.5,'Bord nu du module : contact PETG uniquement')]:ax.annotate(text,(0 if yy>30 else 5.9,yy),(19,yy),arrowprops={'arrowstyle':'->'},fontsize=9)
  ax.axhline(47,color=GRAY,ls='--');ax.text(8,47.3,'Dessous couvercle Z47')
  ax.set(xlim=(-10,62),ylim=(20,50));ax.set_aspect('equal');ax.axis('off')
  f.text(.1,.15,'Vue schématique, patin Click représenté. Les patins MKI suivent leurs deux extrémités courtes.\nPlage PCB haut Z24,75..29,15 ; jeu latéral nominal 0,2 mm par face.\nApprocher doucement au contact, puis reculer ~1/5 tour (0,1 mm de vis).\nVérifier manuellement le faible soulèvement libre et la retenue avant alimentation.\nLe jeu de poche de l’écrou et les tolérances FDM s’ajoutent au réglage ; aucune précharge forte.',fontsize=10)
  finish(pdf,f,'coupe-patin.png')
  f,ax=plt.subplots(figsize=(11.69,8.27));title(f,'Montage du coffret conservé — accès et quincaillerie')
  ax.axis('off');txt='''Ordre de montage recommandé (hors tension)

1  Charger les neuf écrous captifs de la base nue avant les cartes.
2  Fixer la Nucleo et le shield : six vis M2×20 nylon / douze écrous pour deux TR2,
    D126 inchangés. Les deux vis / quatre écrous restants sont la réserve.
3  Installer les supports Adafruit et les étriers selon les coupes précédentes.
4  Poser les cassettes, les deux vitres 50×18×1,5 et les serre-câbles garnis.
    Bride nappe V2.5 : position rapprochée de 2,5 mm de la cassette nord.
5  Contrôler SD insérée / retirée, CN15, ST-LINK, USER, bornier et sortie nappe.
6  Fermer le couvercle ARNACO monobloc et les trappes. Aucun indice IP revendiqué.

17 M3×12 / boîtier, écrous DIN934 :
4 couvercle · 3 bras Nucleo · 2 vitre · 2 bride nappe · 2 bride câble
2 trappe outil RS485 · 1 USER · 1 trappe SD

Ajouter par TR2 :
4 M2×30 et 8 écrous M2 pour FRAM / SD ;
4 M2×30 et 8 écrous M2 pour les deux étriers ;
4 M3×12 et 4 écrous frein DIN985 M3 pour les quatre patins.

Le corps de chaque femelle 1×10 reste entier ; ne neutraliser que la queue
supplémentaire prévue dans l’assemblage électronique. Aucune découpe du corps.
Aucune mesure antérieure de hauteur H n’est exigée pour lancer l’impression.
La mise sous tension attend le contrôle à froid des pièces réellement reçues.'''
  ax.text(.03,.97,txt,va='top',fontsize=11,linespacing=1.5)
  finish(pdf,f)
 # Actual own 3D frame geometry over supplier-derived planar boards.
 f=plt.figure(figsize=(12,9));ax=f.add_subplot(111,projection='3d');title(f,'V2.6 — géométrie propre des étriers et des patins (H11 illustratif)')
 for name,s in v.placed_new():
  vs,fs=s.tessellate(.08,.25);verts=[(p.x,p.y,p.z)for p in vs];faces=[[verts[i]for i in q]for q in fs]
  ax.add_collection3d(Poly3DCollection(faces,facecolor=ORANGE if'MKI'in name else BLUE,edgecolor='none',alpha=.95))
 for x,y,X,Y,z in [(20,17,110,117,13.85),(43.612906,22.511306,61.611854,53.438346,26.45),(83.41,78.73,140.56,104.13,26.45)]:
  ax.add_collection3d(Poly3DCollection([[(x,-y,z),(X,-y,z),(X,-Y,z),(x,-Y,z)]],facecolor=GREEN,alpha=.22,edgecolor=GREEN))
 ax.set(xlim=(15,145),ylim=(-125,-7),zlim=(10,49),xlabel='X',ylabel='−Y',zlabel='Z');ax.view_init(28,-58);ax.set_box_aspect((130,118,39));f.savefig(P/'vues/etriers-3D.png',dpi=160,bbox_inches='tight');plt.close(f)
 print('PDF 5 pages and 5 PNG generated',flush=True)
if __name__=='__main__':main()
