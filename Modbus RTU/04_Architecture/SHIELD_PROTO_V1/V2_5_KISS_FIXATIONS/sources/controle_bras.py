"""Verify metal pivots through the unchanged PETG arms in actual placement."""
import json
import cadquery as cq
import controle as c
P=c.P;m=c.m
local=cq.importers.importStep(str(P/'cad/15_retenue_Nucleo.step')).val();out=[]
for j,((px,py),(ax,ay))in enumerate(zip(m.C['case_keeper_pivots'],m.C['anchors'])):
    right=j==2;contact=ax+5.6 if right else ax-5.3;distance=abs(px-contact);lp=20-distance
    arm=local.rotate((0,0,0),(0,0,1),180 if right else 0).translate((px+lp if right else px-lp,-py,1.4))
    for L in [11.5,12,12.5]:
        s=m.cyl(px,py,4.4-L,1.5,L).fuse(m.cyl(px,py,4.4,3,3.2));v=abs(s.intersect(arm).Volume())
        out.append({'pivot':[px,py],'L':L,'intersection_mm3':v});assert v<1e-6
(P/'preuves/bras-pivots.json').write_text(json.dumps({'checks':out,'unchanged_PETG_arms':True,'passed':True,'scope':'Metal through-slot clearance, no screw head preload on PCB; strength not tested'},indent=2)+'\n')
print('Nine arm/pivot cases PASS; original PETG arms unchanged')
