#!/usr/bin/env python3
"""Targeted editorial update; reuse original PDF vectors and lossless R5 PNGs."""
from pathlib import Path
import sys,subprocess,hashlib,json,argparse
try:import fitz
except ImportError:
 sys.path.insert(0,'/home/lolo/dev/msm/cache/r5-bv-pk-pdf-deps');import fitz
ROOT=Path(__file__).resolve().parents[4]
PDF=Path('Modbus RTU/02_Validation/Rapports/MSM_TR2_Rapport_capacites_vibratoires_31-10-2026.pdf')
SOURCE_REF='a9077dcf'
PROOF=Path('Modbus RTU/00_gouvernance/Echanges_Codex/preuves')
R5=PROOF/'TR2-20261031-R5-ANALYSE-PC-MACHINE-6H'
WM=PROOF/'TR2-20261031-R5-WM-CONFORT-INDICATIF-OFFLINE'
NAVY=(.085,.18,.265);TEAL=(0,.45,.52);MUTED=(.34,.43,.52);PALE=(.935,.968,.977)
FONT='/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf';BOLD='/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf'
GITHUB='https://github.com/loloLR17/MSM_vibe/blob/main/'
BV='https://rulesexplorer-docs.bureauveritas.com/documents/nr467/jul2026/467-NR_PartF_2026-07.pdf'


def sha(b):return hashlib.sha256(b).hexdigest()
def fonts(p):
 p.insert_font(fontname='R',fontfile=FONT);p.insert_font(fontname='B',fontfile=BOLD)
def text(p,rect,s,size=10.2,bold=False,color=NAVY,align=0):
 rect=fitz.Rect(rect);assert p.rect.contains(rect),rect
 remaining=p.insert_textbox(rect,s,fontname='B' if bold else 'R',fontfile=BOLD if bold else FONT,fontsize=size,color=color,lineheight=1.28,align=align)
 if remaining<0:raise ValueError(f'text overflow page{p.number+1}: {s[:75]} missing{-remaining:.2f}pt')
def heading(p,title,subtitle=None):
 fonts(p);w=p.rect.width;p.draw_rect(fitz.Rect(0,0,w,46),color=None,fill=NAVY)
 text(p,(55,17,w-55,34),'MSM / TR2 — CAPTEUR DE VIBRATIONS                                      31.10.2026 · V2',8,bold=True,color=(1,1,1))
 text(p,(55,69,w-45,105),title,17.5,True)
 if subtitle:text(p,(55,110,w-55,141),subtitle,9.8,color=MUTED)
def clear(p,rect):
 p.add_redact_annot(fitz.Rect(rect),fill=(1,1,1));p.apply_redactions(images=0,graphics=2)
def footer(p,n):
 fonts(p);w,h=p.rect.width,p.rect.height
 clear(p,(0,h-58,w,h))
 p.draw_line(fitz.Point(55,h-50),fitz.Point(w-55,h-50),color=(.78,.84,.88),width=.5)
 text(p,(55,h-39,w-90,h-18),'TR2 · Rapport de capacités · Édition illustrée V2 · Temps relatif',7.5,color=MUTED)
 text(p,(w-82,h-39,w-55,h-18),str(n),8,bold=True,align=2)
def uri(p,rect,url):p.insert_link(dict(kind=fitz.LINK_URI,from_=fitz.Rect(rect),uri=url))
def link(p,rect,url):p.insert_link({'kind':fitz.LINK_URI,'from':fitz.Rect(rect),'uri':url})

def cover(doc):
 p=doc.new_page(width=595.276,height=841.89);heading(p,'TR2 — PREMIÈRE CAMPAGNE RÉELLE','Présentation au Chef, au Commandant et à l’Armement')
 text(p,(55,150,540,192),'Lieu de mesure : plancher du PC machine\nLocal occupé en permanence en navigation',12,True,color=TEAL)
 text(p,(55,204,540,247),'Le prototype TR2 a enregistré les vibrations sur trois axes en un point du Mont St Michel. Deux lectures complémentaires sont restituées : le confort vibratoire et l’analyse technique des fréquences.',10.4)
 for x,width,value,label in [(55,235,'≈6 h 06','signal exploité · temps relatif MCU'),(305,235,'58,55 M','triplets XYZ archivés')]:
  p.draw_rect(fitz.Rect(x,261,x+width,325),fill=PALE,color=None);p.draw_rect(fitz.Rect(x,261,x+width,265),fill=TEAL,color=None)
  text(p,(x+14,275,x+width-10,309),value,19,True,color=TEAL);text(p,(x+14,303,x+width-10,320),label,8.3,color=MUTED)
 text(p,(55,342,540,365),'Wm — maximum des RMS d’axes par fenêtre de 60 secondes',10.6,True)
 for x,value,label in [(55,'1,224','Médiane · mm/s'),(220,'2,082','P95 · mm/s'),(385,'2,775','Maximum · mm/s')]:
  p.draw_rect(fitz.Rect(x,375,x+155,443),fill=PALE,color=None)
  text(p,(x+12,385,x+145,420),value,21,True,color=TEAL);text(p,(x+12,419,x+145,438),label,8.8)
 text(p,(55,454,540,493),'365 fenêtres complètes. Le maximum est celui d’une fenêtre de 60 s, pas une crête instantanée. Le P95 décrit la distribution des fenêtres, sans attribuer un état de confort à chaque instant.',9.2)
 text(p,(55,507,540,531),'Repères documentaires Bureau Veritas — même unité : mm/s',10.5,True,color=TEAL)
 text(p,(55,537,540,570),'Catégorie : “Engine control room or switchboard room\n(if continuously manned at sea)”',9.5)
 p.draw_rect(fitz.Rect(55,579,540,631),fill=(.96,.965,.975),color=None)
 for x,label,value in [(71,'Grade 1','4,0'),(230,'Grade 2','4,5'),(389,'Grade 3','5,0')]:
  text(p,(x,584,x+140,601),label,8.5,color=MUTED);text(p,(x,603,x+140,629),value+' mm/s',15,True)
 text(p,(55,641,540,670),'Source : NR467 Part F, juillet 2026, chapitre 6, section 3, tableau 4.\nRepères de grades RMS globaux pondérés 1–80 Hz, pas des alertes sanitaires.',8.7,color=MUTED);link(p,(55,641,540,670),BV+'#page=247')
 text(p,(55,684,540,723),'Lecture indicative d’un prototype non étalonné : ces repères et les valeurs TR2 présentent une capacité de mesure, sans classement BV du PC machine.',9.3,True)
 text(p,(55,738,540,770),'À poursuivre : campagnes répétées au même emplacement et journal d’exploitation synchronisé pour expliquer les variations et les fréquences.',9.3)
 return p

def edit_wm(p):
 fonts(p);clear(p,(50,480,550,714));text(p,(55,484,540,510),'Statistiques du maximum des RMS d’axes — Wm équivalent',11.2,True,color=TEAL)
 p.draw_rect(fitz.Rect(55,518,540,609),fill=PALE,color=None)
 for x,label in [(68,'Fenêtres'),(189,'Médiane'),(303,'P95'),(415,'Maximum')]:text(p,(x,526,x+110,544),label,9,True)
 for y,values in [(551,['60 s · 365','1,224 mm/s','2,082 mm/s','2,775 mm/s']),(578,['120 s · 182','1,229 mm/s','2,078 mm/s','2,458 mm/s'])]:
  for x,value in zip([68,189,303,415],values):text(p,(x,y,x+118,y+23),value,9.4)
 text(p,(55,623,540,664),'La courbe ci-dessus conserve le regroupement énergétique de cinq minutes de l’édition précédente. Le tableau décrit les fenêtres complètes de 60 / 120 s, sans nouvelle analyse du signal.',9.8)
 text(p,(55,679,540,722),'v_w = 0,028 × a_w, avec a_w en mm/s² : cette représentation en vitesse dépend de la même accélération pondérée ; ce n’est pas une intégration indépendante.',9.4,color=MUTED)

def figure(doc,name,title,description):
 p=doc.new_page(width=841.89,height=595.276);fonts(p)
 p.draw_rect(fitz.Rect(0,0,p.rect.width,26),fill=NAVY,color=None)
 text(p,(24,8,810,23),'MSM / TR2 · CAMPAGNE PC MACHINE · FIGURE R5 ORIGINALE · ÉDITION V2',8,True,color=(1,1,1))
 text(p,(24,36,817,59),title,14,True)
 pix=fitz.Pixmap(ROOT/R5/name);available=fitz.Rect(24,65,817,546)
 scale=min(available.width/pix.width,available.height/pix.height);w,h=pix.width*scale,pix.height*scale
 rect=fitz.Rect(available.x0+(available.width-w)/2,available.y0+(available.height-h)/2,available.x0+(available.width+w)/2,available.y0+(available.height+h)/2)
 xref=p.insert_image(rect,filename=str(ROOT/R5/name),keep_proportion=True)
 text(p,(24,553,817,576),'Source : Codex · R5 ANALYSE-PC-MACHINE-6H / '+name+'\n'+description,7.8,color=MUTED)
 link(p,(24,553,817,576),GITHUB+str(R5/name).replace(' ','%20'))
 # Landscape footer occupies only the bottom strip; preserve image/caption.
 p.draw_line(fitz.Point(24,581),fitz.Point(817,581),color=(.78,.84,.88),width=.4)
 return dict(page=p.number+1,file=str(R5/name),source_sha256=sha((ROOT/R5/name).read_bytes()),width=pix.width,height=pix.height,xref=xref,rect=list(rect),effective_dpi=pix.width/(rect.width/72))

def sources(doc,initial):
 p=doc.new_page(width=595.276,height=841.89);heading(p,'SOURCES ET FIGURES INTÉGRÉES','Liens cliquables · séries et figures d’origine conservées')
 refs=[('[S1] Analyses techniques 2–25 Hz','RMS / crêtes / PSD / spectrogramme : preuves et métriques R5.',GITHUB+str(R5/'README.md').replace(' ','%20')),
 ('[S2] Calcul Wm local','Méthode, 60/120 s, tests et contrôles temporels / FFT.',GITHUB+str(WM/'README.md').replace(' ','%20')),
 ('[S3] Rapport Wm durable','Résultat numérique et limites — commit 36c9a4ac.',GITHUB+'Modbus%20RTU/00_gouvernance/Echanges_Codex/archives/TR2-20261031-R5-WM-CONFORT-INDICATIF-OFFLINE/RAPPORT.md'),
 ('[S4] Bureau Veritas','NR467 Part F, juillet 2026, ch. 6, section 3, tableau 4.',BV+'#page=247'),
 ('[S5] ST : électronique','IIS3DWB DS12569 Rev 8 ; carte STEVAL-MKI208V1 / kit V1K.','https://www.st.com/resource/en/datasheet/iis3dwb.pdf'),
 ('[S6] Références de pondération','ISO / GOST : éditions, relations accessibles et réserves.',GITHUB+'experiments/r5_wm_offline/REFERENCES.md')]
 y=151
 for title,desc,url in refs:
  text(p,(55,y,540,y+20),title,10.2,True,color=TEAL);text(p,(55,y+22,540,y+45),desc,9.3);link(p,(55,y,540,y+45),url);y+=59
 text(p,(55,516,540,541),'Trois figures spectrales originales incorporées',11.5,True,color=TEAL)
 text(p,(55,549,540,601),'p. 4 : 02_spectrogram.png · p. 5 : 03_spectra_ridges.png\np. 6 : 04_ridge_power.png\nImages PNG natives, couleurs, légendes, unités et proportions conservées.',9.5)
 text(p,(55,614,540,661),'Les courbes vectorielles Wm et RMS des pages 2–3 proviennent du PDF antérieur : regroupements énergétiques de cinq minutes, conservés sans nouveau traitement. Le schéma de trois fréquences a été remplacé par les figures réelles.',9.4)
 text(p,(55,679,540,726),'Archive : E6N0005.TR2 · 58 552 136 triplets XYZ\nSHA256 physique annoncé :\n91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5',8.1)
 text(p,(55,737,540,772),'PDF source : main '+initial[:12]+' · Édition illustrée V2 du 31/10/2026.\nLe lieu et son occupation permanente sont précisés par le contrat de cette édition.',8.5,color=MUTED)
 return p

def build(evidence):
 evidence=Path(evidence);evidence.mkdir(parents=True,exist_ok=True)
 original=subprocess.check_output(['git','show',SOURCE_REF+':'+str(PDF)],cwd=ROOT);src=fitz.open(stream=original,filetype='pdf');assert len(src)==8
 # Verify display values solely against published aggregates, no signal computation.
 wm=json.loads((ROOT/WM/'summary.json').read_text())
 for width,expected in [(60,['1,224','2,082','2,775']),(120,['1,229','2,078','2,458'])]:
  row=next(r for r in wm if r['name']=='MAIN' and r['clock']=='MCU' and r['width_s']==width)
  assert [format(row[k]*.028,'.3f').replace('.',',') for k in ['P50','P95','maximum']]==expected
 doc=fitz.open();cover(doc)
 doc.insert_pdf(src,from_page=1,to_page=2);edit_wm(doc[1])
 figures=[figure(doc,'02_spectrogram.png','3 | LE SPECTROGRAMME RÉEL 2–25 Hz','Couleur : densité spectrale ; temps et fréquence estimés. Aucun événement de navigation attribué.'),figure(doc,'03_spectra_ridges.png','4 | SPECTRES PSD ET FRÉQUENCES CANDIDATES','Trois périodes faible / médiane / forte ; les fréquences suivies ne désignent pas des machines.'),figure(doc,'04_ridge_power.png','5 | PUISSANCE DES BANDES CANDIDATES','Suivi des bandes autour de 19,57 / 20,63 / 24,37 Hz estimés ; interruptions conservées.')]
 doc.insert_pdf(src,from_page=4,to_page=6)
 p=doc[6];fonts(p);clear(p,(50,591,550,657));text(p,(55,595,540,658),'Archive E6N0005.TR2 : environ 6 h 06 de temps relatif, 58 552 136 triplets et 351 312 816 octets utiles. Fichier préalloué de 640 Mio, queue inutilisée exclue. Fixation au plancher du PC machine ; les axes capteur restent distincts des axes navire, dont la correspondance exacte n’est pas documentée.',10)
 p=doc[8];fonts(p);clear(p,(50,472,555,526));text(p,(55,476,540,529),'Les repères RMS globaux pondérés du tableau 4 BV concernent la catégorie indiquée en première page. Ils sont distincts des composantes crêtes BV-PK (tableaux 5/6), dont le blocage historique demeure. Ce sont des références documentaires de grades, pas des seuils sanitaires.',9.4)
 sources(doc,subprocess.check_output(['git','rev-parse',SOURCE_REF],cwd=ROOT,text=True).strip())
 for i,p in enumerate(doc):
  if i in [3,4,5]:text(p,(775,582,817,595),str(i+1),7.5,True,align=2)
  else:
   p.set_mediabox(fitz.Rect(0,0,595.276,841.89));footer(p,i+1)
 doc.set_metadata(dict(title='MSM TR2 — Capacités vibratoires — Rapport illustré V2',author='Projet MSM / Codex',subject='Campagne PC machine du 31 octobre 2026 : Wm, RMS et figures spectrales originales',keywords='TR2, Wm, RMS, spectrogramme, PSD, PC machine, V2',creator='PyMuPDF — édition documentaire ciblée'))
 doc.set_toc([[1,'Synthèse et repères documentaires',1],[1,'Confort Wm',2],[1,'Intensité technique 2–25 Hz',3],[1,'Spectrogramme original',4],[1,'PSD et raies candidates',5],[1,'Puissance des bandes candidates',6],[1,'Annexe A — Électronique',7],[1,'Annexe B — Méthodes',8],[1,'Annexe C — Validation et limites',9],[1,'Sources et traçabilité',10]])
 doc.subset_fonts();output=ROOT/PDF;temp=evidence/'candidate.pdf';doc.save(temp,garbage=4,deflate=True);doc.close()
 output.write_bytes(temp.read_bytes())
 (evidence/'build.json').write_text(json.dumps(dict(source_ref=SOURCE_REF,source_pdf_sha256=sha(original),output_path=str(PDF),output_sha256=sha(output.read_bytes()),bytes=output.stat().st_size,pages=10,original_figures=figures,method='original PDF vector pages reused;3 full lossless PNG native dimensions; no recomputed series',pymupdf=fitz.VersionBind),indent=2)+'\n')
 print('Built',output.stat().st_size,'bytes /10pages /3original figures')
if __name__=='__main__':
 ap=argparse.ArgumentParser();ap.add_argument('evidence');a=ap.parse_args();build(a.evidence)
