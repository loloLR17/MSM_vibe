"""Check explicit acceptance envelopes against actual exported own STEP parts.

These are geometric acceptance gauges, not measurements of user cables/fingers.
Doors/caps are removed for each service, never silently removed from closed CAD.
"""
import json, math
import cadquery as cq
import modele as m
P=m.P
parts={f.stem:cq.importers.importStep(str(f)).val()for f in (P/'cad').glob('*.step')if f.stem!='assemblage-pieces-propres'}
struct=['01_base','02_couvercle_ARNACO','03_cassette_nord','04_cassette_sud','05_cassette_SD','06_cassette_RS485','07_cadre_vitre']
probes=[('CN1_cable_acceptance_20x12',m.box(60.716,-25,-4,80.716,-7,8)),
('CN15_cable_acceptance_20x12',m.box(57.02,126.7,-4,77.02,151,8)),
('SD_card_release_corridor',m.box(0,88.3,25.85,23.1,103.5,28.85)),
('USER_finger_10x10_acceptance',m.box(24.52,120.5,10.3,34.52,149,20.3)),
('RS485_external_passage',m.box(158,83,28,170,101,34)),
('RS485_tool_vertical_acceptance',m.box(135,84,35,139,100,55)),
('ribbon_port_8x2_acceptance',m.box(49,-22,42,57,-15,44))]
rows=[]
def bounds(s):
    b=s.BoundingBox();return[b.xmin,b.xmax,b.ymin,b.ymax,b.zmin,b.zmax]
def ov(a,b):return all(min(a[2*k+1],b[2*k+1])-max(a[2*k],b[2*k])>1e-8 for k in range(3))
part_bounds={n:bounds(s)for n,s in parts.items()}
for name,probe in probes:
    hits=[]
    for n in struct:
        if not ov(part_bounds[n],bounds(probe)):continue
        v=abs(parts[n].intersect(probe).Volume())
        if v>1e-6:hits.append({'part':n,'volume_mm3':v})
    rows.append({'gauge':name,'positive_intersections':hits,'passed':not hits})
# Seventeen M3 shafts/heads per kit (3x12mm +14x16mm); explicit design limit, not vendor proof.
m3=[]
for x,y in m.C['lid_screw_centres']:m3.append((f'lid_{x}_{y}',m.cyl(x,y,34,1.5,16).fuse(m.cyl(x,y,50,3,3))))
for x,y in m.C['case_keeper_pivots']:m3.append((f'keeper_{x}_{y}',m.cyl(x,y,-7.6,1.5,12).fuse(m.cyl(x,y,4.4,3,3))))
for x in [127.5,149.5]:m3.append((f'tool_{x}',m.cyl(x,92.5,34,1.5,16).fuse(m.cyl(x,92.5,50,3,3))))
for y in [82,101]:m3.append((f'RS_{y}',m.cyl(154,y,14,1.5,16).fuse(m.cyl(154,y,30,3,3))))
for x,z in [(30.5,20),(85,20),(42.5,38),(64.5,38)]:
    s=cq.Solid.makeCylinder(1.5,16,cq.Vector(x,18.2,z),cq.Vector(0,-1,0))
    s=s.fuse(cq.Solid.makeCylinder(3,3,cq.Vector(x,18.2,z),cq.Vector(0,1,0)));m3.append((f'north_{x}_{z}',s))
s=cq.Solid.makeCylinder(1.5,16,cq.Vector(45,-146.8,27),cq.Vector(0,1,0)).fuse(cq.Solid.makeCylinder(3,3,cq.Vector(45,-146.8,27),cq.Vector(0,-1,0)));m3.append(('USER',s))
s=cq.Solid.makeCylinder(1.5,16,cq.Vector(7.2,-96,43),cq.Vector(1,0,0)).fuse(cq.Solid.makeCylinder(3,3,cq.Vector(7.2,-96,43),cq.Vector(-1,0,0)));m3.append(('SDdoor',s))
assert len(m3)==17
fastener_hits=[]
for label,s in m3:
    # Local generic keepers/D126/washers have no global placement in per-part
    # exports, so only global-coordinate structural parts are compared here.
    for n in struct+['08_trappe_USER','09_trappe_SD','10_serre_nappe','11_serre_cable_RS485','12_trappe_outil_RS485']:
        if not ov(bounds(s),part_bounds[n]):continue
        v=abs(s.intersect(parts[n]).Volume())
        if v>1e-6:fastener_hits.append({'fastener':label,'part':n,'mm3':v})
# Nominal added M2 module hardware: three AF4/H1.6 nuts per bolt.
hardware=[]
for x,y in m.C['module_supports_FRAM']+m.C['module_supports_SD']:
    for z in [20.25,21.85,26.45]:
        s=m.hexhole(x,y,z,4,1.6).cut(m.cyl(x,y,z-.1,1,1.8));hardware.append(s)
    hardware.append(m.cyl(x,y,12.25,1,20))
sdprobe=probes[2][1];hardware_hits=[]
for j,s in enumerate(hardware):
    v=abs(s.intersect(sdprobe).Volume())
    if v>1e-6:hardware_hits.append({'hardware':j,'mm3':v})
out={'gauges':rows,'M3_shaft_head_design_limits':{'shafts':17,'diameter':3,'M3x12_count':3,'M3x16_count':14,'head_max_diameter':6,'head_max_height':3,'positive_intersections_with_global_parts':fastener_hits},'SD_added_M2_hardware_intersections':hardware_hits,
 'top_SD_nut_to_card_corridor_minimum_analytical_mm':88.3-(85.68+4/math.sqrt(3)),
 'scope':'Own shell/panels only; real plugs/eye/IDC/cable/finger still not measured. USER probe stops before actual button; no force/course claim. Tool cap and service doors removed. Nominal M2 module hardware only.',
 'passed':all(r['passed']for r in rows)and not hardware_hits and not fastener_hits}
(P/'preuves/services.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out,indent=2),flush=True)
assert out['passed']
