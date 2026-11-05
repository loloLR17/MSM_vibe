from pathlib import Path
import cadquery as cq,json
R=Path(__file__).resolve().parents[2];P=R/'V2_0_EMBOITEMENT';E=Path('/home/lolo/dev/msm/reports/TR2-shield-v20-20261105');G=12.25;Z=G+1.6
sd=cq.importers.importStep(str(R/'mechanical/V1_5/models/Adafruit_4682.step')).val().rotate((0,0,0),(0,0,1),90).translate((43.04,-108.54,Z+11))
own=cq.importers.importStep(str(P/'cad/own-parts-G12p25.step')).val();hits=[];checks=0
for i,s in enumerate(own.Solids()):
 b=s.BoundingBox()
 # Existing V15 retaining structures identified by their global z span/XY bbox.
 if b.zlen<20:continue
 checks+=1;v=s.intersect(sd).Volume()
 if v>1e-6:hits.append({'own_solid':i,'volume_mm3':v})
cq.exporters.export(sd,str(P/'cad/SD4682-manufacturer-placed.step'));bb=sd.BoundingBox()
result={'source':'existing pinned Adafruit4682 STEP V15, rigid transform reused; no new vendor model guessed','G':G,'module_bottom':Z+11,'bbox':[bb.xmin,bb.xmax,bb.ymin,bb.ymax,bb.zmin,bb.zmax],'source_shape_valid':sd.isValid(),'retainer_solids_checked':checks,'positive_intersections':hits,'scope':'actual SD model vs V15 analytical retainers, does not verify actual card extraction/fingers/cable or model revision on user board'};(P/'preuves/SD4682-contre-retenues.json').write_text(json.dumps(result,indent=2)+'\n');print(result)
