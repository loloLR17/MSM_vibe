"""Generate dimensioned review views, not manufacturing drawings for an enclosure."""
from pathlib import Path
import json,html,struct,math
P=Path(__file__).resolve().parents[1];D=json.loads((P/'design.json').read_text());G=json.loads((P/'docs/pcb_geometry.json').read_text());M=json.loads((P/'mechanical/interfaces.json').read_text())
S=['<svg xmlns="http://www.w3.org/2000/svg" width="210mm" height="185mm" viewBox="-15 -25 210 185">','<rect x="-15" y="-25" width="210" height="185" fill="white"/>','<g font-family="sans-serif" font-size="2.4" stroke-width="0.25">']
def rect(x,y,w,h,fill,stroke='#444',dash=False):S.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="{fill}" stroke="{stroke}"'+(' stroke-dasharray="1.5,1"'if dash else'')+'/>')
def text(x,y,t,color='#111',size=2.4):S.append(f'<text x="{x}" y="{y}" fill="{color}" font-size="{size}">{html.escape(t)}</text>')
def line(a,b,color='#444',dash=False):S.append(f'<path d="M{a[0]},{a[1]} L{b[0]},{b[1]}" stroke="{color}" fill="none"'+(' stroke-dasharray="1,1"'if dash else'')+'/>')
text(-10,-19,'TR2 SHIELD V1.2 - INTERFACES / ECHELLE 1:1',size=3.2);text(-10,-14,'NE PAS FABRIQUER : C05 / J5 femelle Morpho et hauteurs a confirmer','#b00020')
rect(21.75,-6.75,70,133.34,'#fafafa','#888',True);text(94,2,'Nucleo : enveloppe UM2861');text(94,6,'Reconstruction, pas CAO C05','#b06000')
rect(20,17,90,100,'#f0fff0','#174d28');text(89,18,'90 x 100 mm')
# Known carrier/module footprint envelopes; the MKI body envelope is explicitly provisional.
for c in D['components']:
 x,y=c['xy_mm'];b={'fram':[-2.54,-2.54,25.4,17.78],'sd':[-20.32,-22.86,22.86,25.4],'dil':[-2.54,-2.54,20.32,33.02],'click':[-21.59,-1.27,57.15,25.4]}.get(c['kind'])
 if b:
  a,z,w,h=b;rect(x+a,y+z,w,h,'#fff1d6'if c['kind']=='dil'else'#dcecf3','#9a6100'if c['kind']=='dil'else'#336070',c['kind']=='dil');text(x+a,y+z-1,c['ref']+' '+c['value'])
for q in G['pads']:
 S.append(f'<circle cx="{q["x"]}" cy="{q["y"]}" r="{q["drill"]/2}" fill="none" stroke="'+('#333'if q['npth']else'#1674a5')+'"/>')
 for_label=q['ref']in('J1','J2','J3','J4','J5')and q['number']=='1'
 if for_label:text(q['x']-3,q['y']-1,q['ref']+'-1',size=1.8)
# Access envelopes represent future enclosure cut requirements, not measured manufacturing cutouts.
rect(0,83,27,26,'none','#0682b4',True);text(-12,80,'SD : poche doigts 27 x 26','#00678c');rect(21.1,88.8,2,14.2,'none','#0682b4',True)
line([22,95.9],[-8,95.9],'#0682b4');text(-12,113,'Retrait -X / aucun etrier devant la carte','#00678c');text(-12,117,'Course fabricant : 0.6 / 1.6 / 5.3 mm',size=2.1)
rect(140,79,35,26,'none','#8d2877',True);rect(132,80,10,24,'none','#8d2877',True);text(113,73,'RS485 : fils +X / tournevis par dessus','#8d2877');line([143,92],[175,92],'#8d2877')
line([118.46,78],[118.46,105],'#d00000',True);text(113,110,'Barriere v101 / v102 a confirmer','#b00020',2.1)
for y in [82,101]:S.append(f'<circle cx="154" cy="{y}" r="1.7" fill="none" stroke="#8d2877"/>')
text(148,115,'Serre-cable sur boitier');text(148,119,'Pas sur le Click')
rect(46,-14,15,35,'none','#8b5a00',True);line([53.5,19],[53.5,-12],'#8b5a00');text(-10,0,'Nappe 12 voies / sortie nord','#8b5a00');text(-10,4,'Selle independante, rayon a confirmer','#8b5a00',2.1)
rect(57,-12,21,17,'none','#666',True);text(94,-5,'USB ST-LINK : maintenance seulement')
rect(22,117,70,14,'none','#f08300',True);text(94,124,'Baie LED1/2/3 + USER / RESET protege','#a06000');text(94,128,'Centres optiques et course a relever C05','#b00020',2.1)
rect(55,122,23,14,'none','#777',True);text(82,136,'CN15 : bloc externe secteur / USB5V + reprise')
line([55,137],[78,137],'#777');text(23,142,'Deux bornes USB sont des ancrages de boitier proposes, pas des percages PCB',size=2.1)
# Retainer through-bolts asymmetry, separate from case-mounted cable anchors.
for h in D['retainer_mount_holes']:
 x,y=h['xy'];text(x+3,y-5 if h['owner']=='J8'and y==20 else y,h['owner']+' '+h['fastener'],size=1.8)
text(-10,149,'Plein : enveloppes sources / PCB. Pointilles : hypotheses et volumes fonctionnels reserves.',size=2.3)
line([0,155],[50,155]);text(53,156,'Controle impression : 50 mm / imprimer100%',size=2.3);S+=['</g></svg>'];svg='\n'.join(S)+'\n';(P/'exports/interfaces-boitier.svg').write_text(svg);(P/'exports/gabarit.svg').write_text(svg)
# Standalone isometric views from the STL exports, not independent reverse-engineered CAD.
S=['<svg xmlns="http://www.w3.org/2000/svg" width="1200" height="720" viewBox="0 0 1200 720">','<rect width="1200" height="720" fill="#f7fafc"/>','<g font-family="sans-serif">','<text x="20" y="30" font-size="23">TR2 V1.2 - modeles de revue / NON LIBERES POUR IMPRESSION DE CAMPAGNE</text>']
for idx,name in enumerate(['mki','click','flat_clamp','round_clamp','user_plunger']):
 raw=(P/f'mechanical/REVIEW_ONLY/{name}.stl').read_bytes();n=struct.unpack_from('<I',raw,80)[0];triangles=[];pts=[]
 for i in range(n):
  v=[struct.unpack_from('<3f',raw,84+50*i+12+j*12)for j in range(3)];pr=[((x+y)*.707107,(x-y)*.408248-z*.816497)for x,y,z in v];pts+=pr;triangles.append((sum(x-y+z for x,y,z in v),pr))
 minx,miny=min(x for x,y in pts),min(y for x,y in pts);maxx,maxy=max(x for x,y in pts),max(y for x,y in pts);scale=min(320/(maxx-minx),245/(maxy-miny));ox=20+(idx%3)*400;oy=75+(idx//3)*315
 S.append(f'<text x="{ox}" y="{oy}" font-size="19">{name} - hypothese nominale</text>')
 for depth,pr in sorted(triangles):
  v=' '.join(f'{ox+30+(x-minx)*scale:.2f},{oy+15+(y-miny)*scale:.2f}'for x,y in pr);S.append(f'<polygon points="{v}" fill="#99bfd4" stroke="#466f86" stroke-width=".18"/>')
S+=['</g></svg>'];(P/'exports/retenues-isometriques.svg').write_text('\n'.join(S)+'\n');print('Generated 1:1 interface plan and isometric views from five compiled meshes')
