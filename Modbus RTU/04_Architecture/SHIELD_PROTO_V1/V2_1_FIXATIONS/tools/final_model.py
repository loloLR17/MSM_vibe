from pathlib import Path
import cadquery as cq,json,math,time
R=Path(__file__).resolve().parents[2];P=R/'V2_1_FIXATIONS';(P/'preuves').mkdir(parents=True,exist_ok=True);(P/'cad').mkdir(exist_ok=True);E=Path('/home/lolo/dev/msm/reports/TR2-shield-v21-20261105');D=json.loads((R/'V1_9_C05/design.json').read_text());T=json.loads((R/'V1_9_C05/preuves/comparaison-synthese.json').read_text())['transform_translation_mm'];source=cq.Shape.importBrep('/home/lolo/dev/msm/reports/TR2-shield-v20-20261105/C05-cache.brep');parts=[x.translate((T[0],-T[1],0)) for x in source.Solids()];boxes=[x.BoundingBox() for x in parts];print('cached1060sourceboxes',flush=True)
def support(radius=3,front=0,bore=1.2,H=12.25):
 s=cq.Solid.makeCylinder(radius,H).cut(cq.Solid.makeCylinder(bore,H+.2,cq.Vector(0,0,-.1)));cut=cq.Workplane('XY').box(10,10,H+2).translate((0,front+5,H/2)).val();return s.cut(cut)
s=support();assert s.isValid();cq.exporters.export(s,str(P/'cad/D126-support.step'));cq.exporters.export(s,str(P/'cad/D126-support.stl'),tolerance=.02,angularTolerance=.08)
(P/'cad/D126-support.scad').write_text('// REVIEW ONLY; mm. Opening toward nearest Zio; rotated180deg at top anchor.\nH=12.25; R=3; bore=2.4;\n$fn=96; difference(){cylinder(h=H,r=R);translate([0,0,-.1])cylinder(h=H+.2,d=bore);translate([-5,0,-1])cube([10,10,H+2]);}\n')
a=cq.Assembly(name='V21_supports_only_review');tests=[]
for j,(x,y)in enumerate(D['mounting_holes_mm']):
 direction=-1 if j==0 else 1
 for case,radius,front,shift,rot in [('nominal',3,0,0,0),('limit_m5',3.1,.1,1.015,-5),('limit_p5',3.1,.1,1.015,5)]:
  body=support(radius,front,1.2).rotate((0,0,0),(0,0,1),(180 if j==0 else 0)+rot).translate((x,-y+direction*shift,0));bb=body.BoundingBox();hits=[]
  for i,(obj,b)in enumerate(zip(parts,boxes)):
   if bb.xmax<=b.xmin or bb.xmin>=b.xmax or bb.ymax<=b.ymin or bb.ymin>=b.ymax or bb.zmax<=b.zmin or bb.zmin>=b.zmax:continue
   v=body.intersect(obj).Volume()
   if v>1e-6:hits.append({'ST_solid':i,'mm3':v})
  tests.append({'hole':j,'xy': [x,y],'case':case,'hits':hits});print(tests[-1],flush=True)
  if case=='nominal':a.add(body,name='support_'+str(j))
a.save(str(P/'cad/implantation-3-supports.step'))
# Explicit clearance arithmetic: no subtraction of the same location uncertainty twice.
float_bound=min((3.33-1.9)/2,(2.93-1.9)/2+.15)+(2.6-1.9)/2
front_bound=.1*math.cos(math.radians(5))+math.sqrt(3.1**2-.1**2)*math.sin(math.radians(5))
clear=2.285-.125-.38-.075-float_bound-front_bound
assert clear>.2
proof={'source':'official C05 BREP from V20,1060solids; original STEP chain unchanged','tests':tests,'nominal_shape':{'name':'D126','outer_radius':3,'bore_diameter':2.4,'height_G':12.25,'opening':'half annulus, flat at bore axis; no thin north wall','minimum_radial_ligament':1.8,'compression_area_mm2':s.Volume()/12.25,'volume_mm3':s.Volume(),'valid':s.isValid()},'tolerance_model':{'Nucleo_hole_max_budget':3.33,'shield_hole_max_budget':2.93,'shaft_min_acceptance':1.9,'relative_hole_center_bound':.15,'bore_max_acceptance':2.6,'max_free_float':float_bound,'profile_Rmax':3.1,'flat_forward_max':.1,'rotation_max_deg':5,'front_projection_max':front_bound,'socket_end_length_allowance':.125,'socket_flash_allowance':.38,'Nucleo_anchor_to_socket_position_allowance':.075,'worst_gap':clear,'guard':.2,'all_actual_parts_within_bounds':'NOT_VERIFIED'},'strength':{'engineering_clamp_load_per_bolt_N':5,'average_stress_at5N_MPa':5/(s.Volume()/12.25),'allowable_stress':'NOT_QUALIFIED material/layer/temperature/creep dependent; 5N is a target not a certified capacity or torque'},'orientation':'opening south at anchor0, north at1/2; maintainwithin5deg atassembly, no demonstrated vibration retention','physical':'NOT_TESTED','fabrication':'NON_LIBERE'};(P/'preuves/geometrie-et-limites.json').write_text(json.dumps(proof,indent=2)+'\n')
