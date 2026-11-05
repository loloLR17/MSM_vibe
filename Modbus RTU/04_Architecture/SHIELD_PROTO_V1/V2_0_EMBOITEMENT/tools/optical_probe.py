from pathlib import Path
import cadquery as cq,json
R=Path(__file__).resolve().parents[2];P=R/'V2_0_EMBOITEMENT';s=cq.Shape.importBrep('/home/lolo/dev/msm/reports/TR2-shield-v20-20261105/C05-cache.brep');T=json.loads((R/'V1_9_C05/preuves/comparaison-synthese.json').read_text())['transform_translation_mm'];solids=[x.translate((T[0],-T[1],0))for x in s.Solids()];rows=[]
for name,x,z in [('LD3',51.72,.8),('LD2',54.92,1.),('LD1',58.12,.8)]:
 # Candidate ray above model LED top, starting1mm north of emitter footprint. NOT actual emitter photometry.
 probe=cq.Solid.makeCylinder(.025,41.92,cq.Vector(x,-19.92,z),cq.Vector(0,1,0));hits=[]
 for i,a in enumerate(solids):
  b=a.BoundingBox()
  if not(b.xmin-.025<=x<=b.xmax+.025 and b.zmin-.025<=z<=b.zmax+.025 and b.ymax>=-19.92 and b.ymin<=22):continue
  v=a.intersect(probe).Volume()
  if v>1e-7:hits.append({'solid':i,'intersection_mm3':v})
 rows.append({'LED':name,'candidate_z':z,'hits':hits,'clear_in_source_model':not hits})
(P/'preuves/rayons-LED.json').write_text(json.dumps({'scope':'three straight northward candidate rays, above footprint model, real ST BREP; not emission direction/visibility or full common observer cone','rows':rows,'window':'common lateral window can span X51..59 and Z0.5..2 but size and clarity NOT_QUALIFIED'},indent=2)+'\n');print(rows)
