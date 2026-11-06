"""Readable A4 PDFs from frozen V27 PNG views and explicit quantities, no CAD call."""
import json,hashlib,math
import fitz
import numpy as np
from PIL import Image
from donnees import *
FONTDIR=Path('/usr/share/fonts/truetype/dejavu')
FONTS={'R':FONTDIR/'DejaVuSans.ttf','B':FONTDIR/'DejaVuSans-Bold.ttf','M':FONTDIR/'DejaVuSansMono.ttf'}
MEASURE={k:fitz.Font(fontfile=str(f))for k,f in FONTS.items()}
BLUE=(.10,.23,.34);INK=(.13,.17,.21);GRAY=(.36,.40,.43);LIGHT=(.93,.96,.97)
TITLE="TR2 V2.7 ABS — dossier d'impression Morvan — 2 prototypes"
LAYOUT=[];TABLE=[]

def line(page,x,y,s,size=11,font='R',color=INK):
 width=MEASURE[font].text_length(s,fontsize=size);assert x>=28 and x+width<=570,(s,x,width)
 assert y-size>=15 and y+size*.3<=823,(s,y)
 page.insert_text((x,y),s,fontname=font,fontsize=size,color=color)
 LAYOUT.append({'page':page.number+1,'text':s,'fontsize':size,'font':font,'x':x,'y':y,'width':width})

def text(page,x,y,w,s,size=11,font='R',color=INK,leading=None):
 lead=leading or size*1.38
 for para in s.split('\n'):
  words=para.split();cur=''
  for word in words:
   if cur and MEASURE[font].text_length(cur+' '+word,fontsize=size)>w:line(page,x,y,cur,size,font,color);y+=lead;cur=word
   else:cur=cur+' '+word if cur else word
  if cur:line(page,x,y,cur,size,font,color);y+=lead
  else:y+=lead*.5
 return y

def page(doc,kind,number,total):
 p=doc.new_page(width=595.276,height=841.89)
 for k,f in FONTS.items():p.insert_font(fontname=k,fontfile=str(f))
 p.draw_rect(fitz.Rect(0,0,595.276,9),fill=BLUE,color=None)
 y=text(p,38,37,520,TITLE,12,'B',BLUE,16)
 line(p,38,y+11,kind,11,'B');line(p,38,815,'V2.7 • aucun ancien ZIP V2.6 • impression prototype ABS',8.5,'R',GRAY)
 line(p,510,815,f'{number}/{total}',9,'B',GRAY)
 return p

def head(p,y,title):
 p.draw_rect(fitz.Rect(38,y-15,557,y+8),fill=LIGHT,color=None);line(p,47,y,title,11,'B',BLUE);return y+24

def table(p,y,rows):
 line(p,44,y,'Fichier exact dans le dossier STL',10,'B',GRAY);line(p,363,y,'Copies',10,'B',GRAY);line(p,418,y,'Reconnaître',10,'B',GRAY);y+=19
 for i,(n,q,role)in enumerate(rows):
  if i%2==0:p.draw_rect(fitz.Rect(38,y-14,557,y+13),fill=(.97,.98,.985),color=None)
  line(p,44,y,n,9.4,'M');line(p,378,y,str(q),12,'B',BLUE)
  # Roles intentionally short, without assembly obligations.
  text(p,418,y,130,role,9.5,leading=11)
  TABLE.append({'PDF':PDFS[0],'page':p.number+1,'filename':n,'quantity':q,'y':y})
  y+=29
 return y

def notice():
 d=fitz.open();p=page(d,'01 — Lire en premier',1,3)
 y=text(p,38,110,515,'Imprimer seulement.\nAucune électronique à monter.',22,'B',leading=30)
 p.draw_rect(fitz.Rect(38,185,557,247),fill=LIGHT,color=None)
 line(p,52,213,'25 fichiers STL → 61 pièces au total',18,'B',BLUE)
 line(p,52,235,'Les quantités incluent déjà les DEUX prototypes.',12,'B',BLUE)
 y=text(p,38,278,515,'Ouvrir les 25 fichiers du dossier STL et suivre les nombres des pages 2 et 3. Un fichier contient une seule pièce : imprimer le nombre indiqué, sans multiplier le lot par deux.',12)
 y=head(p,y+23,'Réglages d’impression')
 for label,body in [
  ('Matière','ABS non chargé, non conducteur. Couleur au choix.'),
  ('Taille et échelle','Fichiers en millimètres, échelle 100 %. Caisse : 158 × 166 × 61 mm. Vérifier le plateau avec la bordure d’adhésion.'),
  ('Orientation','Conserver la position déjà prévue dans les STL ; ne pas redresser automatiquement toutes les pièces.'),
  ('Profil','Profil ABS éprouvé de la machine, enceinte et adhésion adaptées. Le retrait des grandes pièces peut demander une reprise locale par le propriétaire.'),
  ('Couches et densité','Caisses/grands volumes : 0,15–0,20 mm, 4 périmètres, 25–35 %. Petites pièces : 0,10–0,15 mm ; selles, cage, patins et poussoirs : 100 %. D126 : 0,10 mm et 100 %.'),
  ('Finition','Supports locaux retirables sous les débords des poussoirs et si nécessaire dans les poches. Nettoyage et léger ébavurage ; pas de nouvel outillage imposé.')]:
  y=text(p,38,y,515,label,11,'B');y=text(p,38,y,515,body,11);y+=4
 assert y<756,y
 text(p,38,778,515,'Pièces en vrac acceptées : pas de tri, d’étiquetage ni d’assemblage demandé. Aucun achat d’accessoires. Le propriétaire fera le montage et les réglages.',10,'B',leading=13)
 p=page(d,'01 — Quantités : pièces courantes et réserves',2,3)
 y=text(p,38,109,515,'POUR LES DEUX PROTOTYPES ENSEMBLE',13,'B',BLUE)
 y=text(p,38,y+7,515,'Chaque nom apparaît une seule fois dans la liste complète des pages 2 et 3.',11)+15
 ordinary=[r for r in WHITELIST if not r[0].startswith(('20_','41_','50_'))and r[0]!='13_D126.stl']
 y=head(p,y,'Pièces courantes — 14 fichiers, 34 exemplaires');y=table(p,y,ordinary)
 y=head(p,y+14,'Entretoises D126 — 1 fichier, 8 exemplaires');y=table(p,y,[r for r in WHITELIST if r[0]=='13_D126.stl'])
 y=text(p,38,y+9,515,'Les 8 D126 comprennent 6 pièces utiles et 2 pièces de réserve. Si mesurable après nettoyage : hauteur 12,15 à 12,35 mm. Aucun essai avec vis ou carte demandé.',11)
 assert y<790,y
 p=page(d,'01 — Quantités : variantes et échantillon',3,3)
 y=text(p,38,108,515,'Imprimer ces variantes aussi : le propriétaire choisira ensuite les longueurs adaptées. Morvan n’a pas à les choisir ni à les trier.',12)+16
 y=head(p,y,'Supports à trois hauteurs — 6 fichiers, 12 exemplaires');y=table(p,y,[r for r in WHITELIST if r[0].startswith('20_')])
 y=head(p,y+10,'Poussoirs à trois longueurs — 3 fichiers, 6 exemplaires');y=table(p,y,[r for r in WHITELIST if r[0].startswith('41_')])
 y=head(p,y+10,'Échantillon ABS — 1 fichier, 1 exemplaire');y=table(p,y,[r for r in WHITELIST if r[0].startswith('50_')])
 y=text(p,38,y+7,515,'Les supports et poussoirs portent 1, 2 ou 3 petits creux pour distinguer leurs versions. Le petit échantillon peut être imprimé d’abord, si souhaité, pour régler le profil ABS.',11)+9
 y=text(p,38,y,515,'TOTAL : 25 fichiers distincts, 61 pièces',16,'B',BLUE)
 y=text(p,38,y,515,'46 utiles + 12 alternatives + 2 réserves D126 + 1 échantillon. Les variantes ne seront pas toutes montées en même temps.',11)
 assert y<791,y
 d.set_metadata({'title':TITLE,'author':'Projet MSM TR2','subject':'Impression seule, 25 STL / 61 exemplaires pour deux prototypes'})
 d.subset_fonts();d.save(P/'pdf'/PDFS[0],garbage=4,deflate=True);d.close()

def image_page_png(page,name,rect,clipfrac=None):
 img=V27/'vues'/name;doc=fitz.open(img);imdoc=fitz.open('pdf',doc.convert_to_pdf());s=imdoc[0].rect
 clip=fitz.Rect(s.x0,s.y0,s.x1,s.y1)
 if clipfrac:clip=fitz.Rect(s.width*clipfrac[0],s.height*clipfrac[1],s.width*clipfrac[2],s.height*clipfrac[3])
 page.show_pdf_page(rect,imdoc,0,clip=clip,keep_proportion=True)
 imdoc.close();doc.close()

def visual():
 d=fitz.open();p=page(d,'02 — Vue d’ensemble : où vont les pièces',1,2)
 text(p,38,102,515,'Un prototype représenté ; les pièces sont séparées pour mieux les reconnaître. Morvan imprime les plastiques et ne monte pas les cartes.',11)
 # Reuse the frozen PNG, clip its old small legend only in PDF, no bitmap/CAD edit.
 name='VUE_ECLATEE_V27.png';im=np.asarray(Image.open(V27/'vues'/name).convert('RGB'));H,W,_=im.shape
 mask=np.any(np.all(im[:,int(W*.025):int(W*.065)]<150,axis=2),axis=1);ys=np.where(mask & (np.arange(H)>H*.07)&(np.arange(H)<H*.92))[0]
 runs=[]
 for v in ys:
  if not runs or v>runs[-1][-1]+3:runs.append([int(v)])
  else:runs[-1].append(int(v))
 runs=[r for r in runs if len(r)>8];assert len(runs)==12,[(min(r),max(r))for r in runs]
 crop=(0,.065,.74,.94);width=385;height=width/((W*.74)/(H*(.94-.065)));rect=fitz.Rect(32,146,32+width,146+height)
 image_page_png(p,name,rect,crop)
 names={1:'Caisse',2:'Façades et fenêtre',3:'Carte Nucleo',4:'Entretoises et retenues',5:'Shield : carte ajoutée',6:'Supports des modules',7:'Mémoire et lecteur SD',8:'Capteur et module',9:'Retenues et patins',10:'Brides de câbles',11:'Guide du poussoir',12:'Couvercle et poussoir'}
 positions=[]
 for n,run in zip(range(12,0,-1),runs):
  cy=(min(run)+max(run))/2;top=min(run);xs=np.where(np.all(im[top:top+3,:,:]<150,axis=2).any(axis=0)[int(W*.025):int(W*.065)])[0]
  cx=(float(xs.min()+xs.max())/2+int(W*.025))if len(xs)else W*.044
  x=rect.x0+cx/(W*.74)*width;y=rect.y0+(cy-H*.065)/(H*(.94-.065))*height
  radius=6.5 if n in [2,3]else 9
  p.draw_oval(fitz.Rect(x-radius,y-radius,x+radius,y+radius),color=BLUE,fill=(1,1,1),width=.8)
  tw=MEASURE['B'].text_length(str(n),fontsize=10);line(p,x-tw/2,y+3.6,str(n),10,'B',BLUE)
  # Large plain-language legend, ordered from lid to base like the view.
  text(p,438,152+(12-n)*44,119,f'{n:02d}  '+names[n],10.5,'B',BLUE,14)
  positions.append({'number':n,'source_pixel_y':cy,'PDF_centre':[x,y]})
 text(p,38,760,515,'Images de la conception : les cartes et accessoires servent seulement à comprendre la forme finale. Aucun achat, essai de bouton ni montage électronique demandé.',10.5)
 p=page(d,'02 — Le résultat une fois assemblé',2,2)
 image_page_png(p,'BOITIER_FERME.png',fitz.Rect(45,111,550,410))
 text(p,44,428,510,'Boîtier fermé : le couvercle et son poussoir sont au-dessus.',12,'B',BLUE)
 image_page_png(p,'IMPLANTATION.png',fitz.Rect(45,447,550,734))
 text(p,44,754,510,'Boîtier ouvert : les pièces plastiques maintiendront les cartes. Assemblage et réglages réalisés plus tard par le propriétaire.',11)
 d.set_metadata({'title':TITLE+' — vues','author':'Projet MSM TR2','subject':'Reconnaissance des pièces, sans instruction de montage'})
 d.subset_fonts();d.save(P/'pdf'/PDFS[1],garbage=4,deflate=True);d.close()
 (P/'preuves/vues-reutilisees.json').write_text(json.dumps({'sources':{n:hashlib.sha256((V27/'vues'/n).read_bytes()).hexdigest()for n in ['VUE_ECLATEE_V27.png','BOITIER_FERME.png','IMPLANTATION.png']},'CAD_regenerated':False,'source_images_modified':False,'exploded_legend':'Existing image clipped only in the PDF viewport; original 12 numbered marks enlarged, plain-language PDF legend','numbered_marks':positions},indent=2,ensure_ascii=False)+'\n')

def main():
 notice();visual();(P/'preuves/table-PDF.json').write_text(json.dumps(TABLE,indent=2,ensure_ascii=False)+'\n');(P/'preuves/mise-en-page.json').write_text(json.dumps(LAYOUT,indent=2,ensure_ascii=False)+'\n')
 print('Notice3pages + visual2pages; all frozen views reused, no CAD/STL change')
if __name__=='__main__':main()
