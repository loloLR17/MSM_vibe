"""1:1 corrected shield + measured-in-CAD ST reference. No fabrication release."""
from pathlib import Path
import json,math
import pymupdf as pdf
P=Path(__file__).resolve().parents[1];B=json.loads((P/'docs/checks/pcb-readback.json').read_text());D=json.loads((P/'design.json').read_text());I=json.loads((P/'preuves/interfaces-ST.json').read_text());MM=72/25.4
xy=lambda x,y:pdf.Point((x+30)*MM,(y+45)*MM)
doc=pdf.open()
for bank in ['TR2-001','TR2-002']:
 page=doc.new_page(width=210*MM,height=297*MM)
 def text(x,y,s,size=8,color=(0,0,0)):page.insert_text((x*MM,y*MM),s,fontsize=size,color=color)
 text(12,12,'V1.9 C05 OFFICIEL - '+bank+' - VUE DESSUS',11)
 text(12,18,'NON LIBERE / NE PAS COMMANDER - remplace le papier V1.4/V1.8',8,(.7,0,0))
 text(12,24,'Taille reelle100%, sans ajustement. Mesurer les DEUX barres100mm.')
 poly=[xy(*p)for p in I['Nucleo_outline_shield_frame_mm']];page.draw_polyline(poly,color=(.5,.5,.5),width=.5)
 poly=[xy(*p)for p in D['board_outline_mm']];page.draw_polyline(poly+[poly[0]],color=(0,.4,0),width=.6)
 for p in B['pads']:
  critical=p['ref']in ['J1','J2','J3','J4','J5'];pos=xy(p['x'],p['y']);color=(0,.15,.7)if critical else(.75,.75,.75)
  if p['ref']=='J5'and p['number']:color=(.8,0,0)if p['number']=='5'else(.75,.35,0)
  page.draw_circle(pos,p['drill']/2*MM,color=color,width=.4)
  if critical and p['number']:
   n=str(int(p['number'])+64)if p['ref']=='J5'else p['number'];page.insert_text(pos+pdf.Point(1.05*MM,-.35*MM),n,fontsize=3.6,color=color)
   if p['number']=='1':page.draw_rect(pdf.Rect(pos-pdf.Point(1*MM,1*MM),pos+pdf.Point(1*MM,1*MM)),color=color,width=.4)
 for c in D['components'][:5]:page.insert_text(xy(c['xy_mm'][0]-2,c['xy_mm'][1]-3),c['ref']+'/'+c['value'],fontsize=5)
 for c in I['components']:
  if c['ref']in ['B1','B2','LD1','LD2','LD3']:
   pt=xy(*c['xy_shield_mm']);page.draw_circle(pt,.5*MM,color=(.65,0,.65),width=.5);page.insert_text(pt+pdf.Point(0,-2*MM),c['ref'],fontsize=5,color=(.65,0,.65))
 text(12,179,'Bleu: shield corrige, centres ST superposes (ecart<=0,000002mm nominal).',7)
 text(12,185,'J1/CN7 +1,016mm enY ; J5 +0,025001mm ; CN8/9/10 conserves.',7)
 text(12,191,'Gris: contour Nucleo ST ODB++, violet: origines empreintes B1/B2/LED.',7)
 text(12,197,'Hors tension, comparer les5interfaces, trois fixations et contour, sans force.',7)
 text(12,203,'J5 femelle2x3: males66/67/68/69, vides65/70 ; seul69/PG4 est DE.',7)
 text(12,209,'Retour unique: deux barresX/Y, banque, centres OK/ecart, fixation OK/ecart.',7)
 text(12,215,'Pas de mesures de toutes broches. Si ecart: une photo orthogonale du papier.',7)
 text(12,221,'Le papier ne valide pas G/J5 enZ, les jeux, contacts, LED ou acces reels.',7)
 text(12,227,'G naturel a verifier avec temoins exacts deja disponibles, sans acheter/forcer.',7)
 text(12,233,'USER simple et RESET protege; CN15/STLINK/SD/RS485/nappe a conserver.',7)
 page.draw_line(pdf.Point(20*MM,250*MM),pdf.Point(120*MM,250*MM),width=.7)
 page.draw_line(pdf.Point(190*MM,135*MM),pdf.Point(190*MM,235*MM),width=.7)
 text(20,257,'100mm X ; 100mm Y a droite. Releves: X=....... Y=.......mm',8)
 text(12,269,'Source: ST MB1549.PcbDoc, projet C/05, BDP4.0; controle ODB/percages.',7)
 text(12,275,'Repere: dessus, X droite/Y bas. NE PAS RETOURNER EN MIROIR.',7)
f=P/'exports/GABARITS_C05_V19_100POURCENT.pdf';doc.save(f,deflate=True);doc.close()
with pdf.open(f)as d:
 assert len(d)==2
 for page in d:
  lines=[(i[1],i[2])for item in page.get_drawings()for i in item['items']if i[0]=='l'];assert sum(abs(math.dist(a,b)/MM-100)<.001 for a,b in lines)>=2
  assert abs(page.rect.width/MM-210)<.001 and abs(page.rect.height/MM-297)<.001
 d[0].get_pixmap(matrix=pdf.Matrix(1.2,1.2)).save('/tmp/v19-gabarit.png')
print('PDF2pagesA4, two100mm calibration vectors per page, top view only')
