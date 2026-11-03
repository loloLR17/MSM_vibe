"""Generate actual-PCB-derived 1:1 templates and an honest, incomplete C05 comparison.
Requires PyMuPDF. Does not read hardware and cannot establish C05 mechanical parity.
"""
from pathlib import Path
import json, csv, hashlib, math
import pymupdf as pdf
P=Path(__file__).resolve().parents[1]
D=json.loads((P/'design.json').read_text());B=json.loads((P/'docs/checks/pcb-readback.json').read_text())
MM=72/25.4
def xy(x,y,mirror=False):return pdf.Point((160-x if mirror else x+30)*MM,(y+45)*MM)
doc=pdf.open();checks=[]
for label,mirror in [('TR2-001 C05 - VUE DESSUS / SUPERPOSITION',False),('TR2-002 C05 - VUE DESSUS / SUPERPOSITION',False),('SHIELD VUE DESSOUS MIROIR - NE PAS SUPERPOSER',True)]:
 page=doc.new_page(width=210*MM,height=297*MM)
 def text(x,y,s,size=8,color=(0,0,0)):page.insert_text((x*MM,y*MM),s,fontsize=size,color=color)
 text(15,15,'TR2 V1.3 - '+label,10)
 text(15,21,'NE PAS COMMANDER - C05 et hauteur male/femelle J5 encore NON VERIFIEES',8,(.7,0,0))
 text(15,27,'Imprimer taille reelle 100%, sans ajustement. Verifier les deux barres de 100 mm.')
 # All holes from the saved board; Nucleo interfaces/anchors highlighted.
 poly=[xy(x,y,mirror)for x,y in D['board_outline_mm']];page.draw_polyline(poly+[poly[0]],color=(0,.35,0),width=.7)
 for p in B['pads']:
  pos=xy(p['x'],p['y'],mirror);critical=p['ref']in('J1','J2','J3','J4','J5')
  color=(.05,.15,.7)if critical else(.7,.7,.7)
  page.draw_circle(pos,p['drill']/2*MM,color=color,width=.35)
  if critical and p['number']=='1':page.draw_rect(pdf.Rect(pos.x-1.05*MM,pos.y-1.05*MM,pos.x+1.05*MM,pos.y+1.05*MM),color=color,width=.5)
  if critical and p['number']:
   page.insert_text(pos+pdf.Point(1.1*MM,-.4*MM),p['number'],fontsize=3.8,color=color)
 for x,y in D['mounting_holes_mm']:
  pos=xy(x,y,mirror);page.draw_circle(pos,2.25*MM,color=(.8,.25,0),width=.4)
 for c in D['components'][:5]:
  x,y=c['xy_mm'];pos=xy(x,y-4,mirror);page.insert_text(pos,c['ref']+' '+c['value'],fontsize=5.2)
 text(15,172,'Bleu : CN7/8/9/10 + J5 CN12-69/70. Carre : broche locale1.')
 text(15,178,'Orange : enveloppe tete/rondelle Nucleo diam.4.5 mm aux trois ancrages. Gris : autres trous.')
 text(15,184,'J5 local1=69/PG4 ; local2=70/PG6 NON CONNECTE. Femelle2 SOUS SHIELD; males Nucleo deja presents.')
 text(15,190,'Les positions viennent du PCB sauvegarde / reconstruction UM2861Rev4, pas de CAO C05.')
 text(15,196,'Controle futur hors tension : tous centres, contour, ancrages, males CN12 existants : section/longueur/hauteur a mesurer.')
 text(15,202,'Le papier ne prouve ni hauteur Zio/J5, ni soudure dessous, ni collisions dans l epaisseur.')
 text(15,208,'Resultat futur : C05 lisible [ ] ; centres [ ] ; 3 trous [ ] ; J5 [ ] ; degagement vis [ ]')
 text(15,214,'Hauteur Zio mesuree : ........ mm ; males69/70 : section ........ / longueur exposee ........ mm')
 text(15,220,'MKI/Click reel : contour/repere/zone ancrage/isolation compatibles [ ] ; photo avec regle [ ]')
 text(15,226,'Aucune manipulation ni assemblage demande pendant la mission documentaire courante.')
 # Orthogonal calibration lengths are actual PDF vectors, not a screen-size assumption.
 page.draw_line(pdf.Point(20*MM,240*MM),pdf.Point(120*MM,240*MM),color=(0,0,0),width=.7)
 page.draw_line(pdf.Point(190*MM,135*MM),pdf.Point(190*MM,235*MM),color=(0,0,0),width=.7)
 text(20,246,'100 mm horizontal ; 100 mm vertical a droite. Mesure impression : .... / .... mm')
 checks.append({'page':len(checks)+1,'mirror':mirror,'scale':1,'calibration_mm':[100,100],'nucleo_numbered_pads':sum(p['ref']in('J1','J2','J3','J4','J5')and bool(p['number'])for p in B['pads'])})
f=P/'exports/gabarit-C05-V1_3.pdf';doc.save(f,garbage=4,deflate=True);doc.close()
# This table exposes the missing manufacturer evidence rather than filling it with self-comparisons.
rows=[]
for p in B['pads']:
 if p['ref']in('J1','J2','J3','J4','J5'):
  rows.append({'ref':p['ref'],'pad':p['number']or'NPTH','shield_x_mm':p['x'],'shield_y_mm':p['y'],'drill_mm':p['drill'],'ST_C05_x_mm':'','ST_C05_y_mm':'','delta_mm':'','source':'saved PCB; UM2861Rev4 reconstruction','status':'C05_NOT_VERIFIED'})
with (P/'docs/comparaison-C05.csv').open('w',newline='')as out:
 w=csv.DictWriter(out,fieldnames=list(rows[0]));w.writeheader();w.writerows(rows)
# Verify page dimensions and calibration vectors after reopening the produced artifact.
with pdf.open(f)as r:
 assert len(r)==3
 for page in r:
  assert abs(page.rect.width/MM-210)<.001 and abs(page.rect.height/MM-297)<.001
  segments=[(it[1],it[2])for d in page.get_drawings()for it in d['items']if it[0]=='l']
  assert sum(abs(math.dist(a,b)/MM-100)<.001 for a,b in segments)>=2
assert len(rows)==105 # 102 header pads + three carrier anchors attached to J1
(P/'docs/checks/preorder-template.json').write_text(json.dumps({'status':'PASS_TEMPLATE_GENERATION_ONLY','pages':checks,'manufacturer_coordinates_known':False,'comparison_rows':len(rows),'sha256':hashlib.sha256(f.read_bytes()).hexdigest(),'physical_check':'NOT_PERFORMED'},indent=2)+'\n')
print('Template PDF3 pages, scale1:1; comparison CSV105 rows; physical fit NOT VERIFIED')
