from pathlib import Path
import cadquery as cq,json
R=Path(__file__).resolve().parents[2];P=R/'V2_1_FIXATIONS';D=json.loads((R/'V1_9_C05/design.json').read_text());T=json.loads((R/'V1_9_C05/preuves/comparaison-synthese.json').read_text())['transform_translation_mm'];s=cq.Shape.importBrep('/home/lolo/dev/msm/reports/TR2-shield-v20-20261105/C05-cache.brep');src=[x.translate((T[0],-T[1],0))for x in s.Solids()];bbs=[x.BoundingBox()for x in src];out=[];a=cq.Assembly(name='V21_fasteners_nominal')
for j,(x,y)in enumerate(D['mounting_holes_mm']):
 shaft=cq.Solid.makeCylinder(1,20,cq.Vector(x,-y,-1.6));head=cq.Solid.makeCylinder(1.9,1.3,cq.Vector(x,-y,-2.9));bolt=shaft.fuse(head);nut=cq.Workplane('XY').polygon(6,4/(3**.5/2)).extrude(1.6).val().cut(cq.Solid.makeCylinder(1,1.8,cq.Vector(0,0,-.1)))
 things=[('nylon_bolt_head_below',bolt),('nylon_nut',nut.translate((x,-y,13.85))),('nylon_jamnut',nut.translate((x,-y,15.45)))];hits=[]
 for n,obj in things:
  bb=obj.BoundingBox();a.add(obj,name=n+str(j))
  for i,(b,part)in enumerate(zip(bbs,src)):
   if bb.xmax<=b.xmin or bb.xmin>=b.xmax or bb.ymax<=b.ymin or bb.ymin>=b.ymax or bb.zmax<=b.zmin or bb.zmin>=b.zmax:continue
   common=obj.intersect(part);vol=common.Volume()
   if vol>1e-6:
    cb=common.BoundingBox();bearing=(i==0 and n=='nylon_bolt_head_below' and cb.zlen<1e-5 and abs(cb.zmax+1.6)<1e-5)
    hits.append({'part':n,'ST_solid':i,'mm3':vol,'z_extent':cb.zlen,'z_max':cb.zmax,'classification':'INTENTIONAL_HEAD_TO_PCB_BEARING_NUMERICAL_FACE_TOLERANCE'if bearing else'UNEXPECTED_INTERSECTION'})
 out.append({'anchor':j,'hits':hits});print(out[-1],flush=True)
a.save(str(P/'cad/visserie-nominale.step'));(P/'preuves/visserie-ST.json').write_text(json.dumps({'cases':out,'head_diameter':3.8,'nut_AF':4,'nut_corner_diameter':4/(3**.5/2),'head_position':'below Nucleo','nylon_nuts':'two on top shield; upper end17.05mm, screw end18.4, modulebottom24.85','shaft_length_under_head':20,'remaining_above_nuts':1.35,'model':'nominal catalogue acceptance dimensions, no threads/strength model or complete positional tolerance proof'},indent=2)+'\n')
