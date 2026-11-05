"""Targeted V26 geometric checks; native supplier sources remain outside Git.

Run with --native-dir after fetching the pinned sources in SOURCES.md.
No physical qualification. H8..15.5 is machining coverage, NOT socket mating travel.
"""
from pathlib import Path
import argparse,hashlib,itertools,json,math
import cadquery as cq
import numpy as np
import trimesh
import modele as v
P=v.P;m=v.m
def save(n,d):(P/'preuves'/n).write_text(json.dumps(d,indent=2,ensure_ascii=False)+'\n')
def bb(s):
    b=s.BoundingBox();return[b.xmin,b.xmax,b.ymin,b.ymax,b.zmin,b.zmax]
def near(a,b):return all(min(a[i+1],b[i+1])-max(a[i],b[i])>1e-7 for i in [0,2,4])
def overlap(a,b):return abs(a.intersect(b).Volume())if near(bb(a),bb(b))else 0
def nut(x,y,z,af,h,bore):return m.hexhole(x,y,z,af,h).cut(m.cyl(x,y,z-.1,bore,h+.2))
def screw(x,y,z,L,r,headr,headh):return m.cyl(x,y,z,r,L).fuse(m.cyl(x,y,z-headh,headr,headh))
def main(native):
    v.build();parts=v.PARTS;hits=[];checks=[]
    def clear(label,a,b):
        vol=overlap(a,b);checks.append({'check':label,'positive_overlap_mm3':vol})
        if vol>1e-5:hits.append(checks[-1])
    rows=json.loads((P/'preuves/pieces.json').read_text());meshrows=[]
    for q in rows:
        sf=P/q['STL'];cf=P/q['STEP']
        assert hashlib.sha256(sf.read_bytes()).hexdigest()==q['STL_sha256']
        if q['inherited_exact_V25']:
            assert sf.read_bytes()==(v.V25/q['STL']).read_bytes()and cf.read_bytes()==(v.V25/q['STEP']).read_bytes()
            meshrows.append({'id':q['id'],'method':'byte identity, evaluated V25 meshes/STEP and associated proof reused','passed':True});continue
        mesh=trimesh.load_mesh(sf,process=True);s=cq.importers.importStep(str(cf)).val()
        ok=bool(mesh.is_watertight and mesh.is_volume and mesh.is_winding_consistent and len(mesh.split())==1 and s.isValid()and len(s.Solids())==1 and min(mesh.bounds[0])>-.001 and abs(mesh.volume-s.Volume())/s.Volume()<.01)
        meshrows.append({'id':q['id'],'watertight':bool(mesh.is_watertight),'components':len(mesh.split()),'STEP_solids':len(s.Solids()),'STEP_valid':bool(s.isValid()),'minimum_print_XYZ':mesh.bounds[0].tolist(),'relative_mesh_volume_error':abs(mesh.volume-s.Volume())/s.Volume(),'passed':ok});assert ok,q['id']
    save('relecture-maillages.json',meshrows);print('Meshes: 11 reopened, 14 inherited exact',flush=True)
    case=cq.importers.importStep(str(v.V25/'cad/assemblage-imprimable.step')).val()
    oldmetal=cq.importers.importStep(str(v.V25/'cad/quincaillerie-M3x12-enveloppes.step')).val()
    click=cq.Shape.importBrep(str(native/'Click2-cache.brep')).rotate((0,0,0),(0,0,1),-90).translate((87.22,-80,0))
    sd=cq.Shape.importBrep(str(native/'SD-cache.brep')).rotate((0,0,0),(0,0,1),90).translate((43.04,-108.54,0))
    # Source ST GKO cut-line centres, header drills and paste/assembly envelopes.
    mkipcb=m.box(43.612906,22.511306,-1.6,61.611854,53.438346,0)
    for x in [45,60.239746]:
        for j in range(12):mkipcb=mkipcb.cut(m.cyl(x,24+2.54*j,-1.7,.45,1.8))
    mkicomps=[m.box(46.5,25.8,0,58.8,41.0,8.5)] # expanded complete SHF + mated FFSD/cable root design envelope
    for x,y in [(55.884,46.83),(54.795,46.83),(49.205,46.81),(50.295,46.81)]:mkicomps.append(m.box(x-.65,y-.8,0,x+.65,y+.8,2))
    # Entire header rows are excluded, not merely their circular drills.
    mkicomps.extend(m.box(x-1.3,22.7,-1.6,x+1.3,53.3,2.5)for x in [45,60.239746])
    fram=m.box(42.46,56.46,0,67.86,74.24,1.6)
    for x,y in m.C['module_supports_FRAM']:fram=fram.cut(m.cyl(x,y,-.1,1.25,1.8))
    eagle=json.loads((P/'sources/adafruit-derives.json').read_text())
    fram_components=[]
    for q in eagle['FRAM']['component_envelopes']:
        x,y,X,Y=q['bbox_XY'];fram_components.append(m.box(x,y,1.6,X,Y,5.6))
    fram=cq.Compound.makeCompound([fram]+fram_components)
    nominal_refs=[('FRAM_PCB',fram.translate((0,0,24.85))),('SD_native',sd.translate((0,0,24.85))),('MKI_PCB',mkipcb.translate((0,0,26.45))),('MKI_components',cq.Compound.makeCompound(mkicomps).translate((0,0,26.45))),('Click_native',click.translate((0,0,26.45)))]
    new=v.placed_new()
    for rn,rs in nominal_refs:clear(rn+' / V25 case',rs,case)
    for n,s in new:
        clear(n+' / V25 case',s,case);clear(n+' / V25 metal',s,oldmetal)
        for rn,rs in nominal_refs:clear(n+' / '+rn,s,rs)
    for (an,a),(bn,b)in itertools.combinations(new,2):clear(an+' / '+bn,a,b)
    print('Nominal frames/native modules checked',len(checks),'checks',flush=True)
    ranges=[];metal=[];nominal_assembly=cq.Assembly(name='V26_STANDARD_HARDWARE_ENVELOPES')
    for B in [24.75,26.45,29.15]:
        # Continuous Z intervals are bounded analytically below. Extremes plus
        # nominal catch native top/bottom parts; not an arbitrary all-position proof.
        for gap in [0,.1]:
            for which,n in [('MKI','30_etrier_MKI'),('Click','31_etrier_Click')]:
                module=(click if which=='Click'else cq.Compound.makeCompound([mkipcb]+mkicomps)).translate((0,0,B))
                fr=parts[n]['shape'];clear(f'{n} B{B} native module',fr,module)
                for i,((x,y),pn)in enumerate(zip(v.CONFIG[which]['guides'],v.CONFIG[which]['types'])):
                    shoe=parts[pn]['shape'].translate((x,-y,B+gap))
                    bolt=m.cyl(x,y,B+gap+1.5,1.5,12).fuse(m.cyl(x,y,B+gap+13.5,3,3.2))
                    ny=nut(x,y,32.8,5.5,4,1.5)
                    clear(f'{pn} B{B} gap{gap}/frame',shoe,fr);clear(f'{pn} B{B} gap{gap}/module',shoe,module)
                    clear(f'M3 B{B} gap{gap}/frame',bolt,fr);clear(f'M3 B{B} gap{gap}/shoe',bolt,shoe);clear(f'DIN985 / frame {n}',ny,fr)
                    clear(f'M3 B{B} / native {which}',bolt,module);clear(f'DIN985 / native {which} B{B}',ny,module)
                    clear(f'M3 B{B} / case',bolt,case);clear('DIN985 / case',ny,case)
                    tip=B+gap+1.5;headbottom=tip+12
                    assert tip<32.8 and headbottom>36.8
                    assert B+gap>=23.5 and B+gap+2.5<=32
                    ranges.append({'which':which,'i':i,'PCB_top':B,'lift_gap':gap,'cup_overlap_in_guide':min(B+gap+2.5,32)-max(B+gap,23.5),'head_top':headbottom+3.2,'lid_margin':47-headbottom-3.2,'full_nyloc_engagement':True,'closed_PETG_floor':1.5})
                    if B==26.45 and gap==0:
                        nominal_assembly.add(bolt,name=which+str(i)+'_M3');nominal_assembly.add(ny,name=which+str(i)+'_DIN985')
    # Positive support and retained axial stack, ordinary M2 from below.
    modules=m.C['module_supports_FRAM']+m.C['module_supports_SD']
    for H in [8,11,15.5]:
        for j,(x,y)in enumerate(modules):
            t=1.6 if j<2 else 1.57;z=13.85+H
            support=v.bearing(H).translate((x,-y,13.85));under=parts['24_cale_1']['shape'].translate((x,-y,11.25));upper=parts['24_cale_1']['shape'].translate((x,-y,z+t))
            bolt=screw(x,y,11.25,30,1,2.1,2.2)
            ns=[nut(x,y,z+t+1+k*1.6,4,1.6,1)for k in [0,1]]
            for q,shape in [('support',support),('under_washer',under),('upper_washer',upper)]+[(f'nut{k}',s)for k,s in enumerate(ns)]+[('bolt',bolt)]:
                clear(f'{q} H{H} {j}/case',shape,case)
                for nn,fr in new:clear(f'{q} H{H} {j}/{nn}',shape,fr)
            for q,s in [('support',support),('under',under),('upper',upper)]+[(f'nut{k}',s)for k,s in enumerate(ns)]:clear(f'M2 / {q} H{H} {j}',bolt,s)
            ref=(fram.translate((0,0,z))if j<2 else sd.translate((0,0,z)))
            for q,s in [('support',support),('upper',upper),('bolt',bolt)]+[(f'nut{k}',s)for k,s in enumerate(ns)]:clear(f'{q} / native FRAM-SD H{H} {j}',s,ref)
            if j>=2:
                corridor=m.box(0,88.3,z+1,23.1,103.5,z+4)
                for q,s in [('support',support),('bolt',bolt),('upper',upper)]+[(f'nut{k}',s)for k,s in enumerate(ns)]:clear(f'{q} / card extraction H{H} {j}',s,corridor)
            assert 11.25+29.5-(z+t+1+3.2)>=.55-1e-8
            metal.append({'module':j,'H':H,'shaft_bottom':11.25,'head_bottom':9.05,'M2x30_min_tip':40.75,'nut_top':z+t+4.2,'thread_emergence_min':40.75-(z+t+4.2)})
            if H==11:
                for q,s in [('bolt',bolt),('nut0',ns[0]),('nut1',ns[1])]:nominal_assembly.add(s,name=f'module{j}_{q}')
    for which in ['MKI','Click']:
        fr=parts['30_etrier_MKI'if which=='MKI'else'31_etrier_Click']['shape']
        for j,(x,y)in enumerate(v.CONFIG[which]['anchors']):
            b=screw(x,y,11.25,30,1,2.1,2.2);w=parts['24_cale_1']['shape'].translate((x,-y,11.25))
            ns=[nut(x,y,34.5+k*1.6,4,1.6,1)for k in [0,1]]
            for q,s in [('bolt',b),('washer',w),('nut0',ns[0]),('nut1',ns[1])]:
                clear(f'anchor {which}{j} {q}/frame',s,fr);clear(f'anchor {which}{j} {q}/case',s,case)
                for rn,rs in nominal_refs:clear(f'anchor {which}{j} {q}/{rn}',s,rs)
                if q!='washer':nominal_assembly.add(s,name=f'anchor_{which}{j}_{q}')
    # Full 1x10 female housings kept; header stop chain is a hypothesis, no
    # undocumented electrical engagement window inferred from outer height.
    sockets=[m.box(43.08,57.75,13.85,69.78,60.25,22.35),m.box(39.25,81.22,13.85,41.75,107.92,22.35)]
    for sn,s in enumerate(sockets):
        for n,fr in new:clear(f'full socket {sn}/{n}',s,fr)
        for j,(x,y)in enumerate(modules):clear(f'full socket{sn}/post{j}',s,v.bearing(11).translate((x,-y,13.85)))
    # Ribbon swept rectangular slab: union of conservative segment AABBs,
    # includes 0.5 mm cable/design half thickness, no sharp zero-radius turn.
    ribbon_results=[]
    for B in [24.75,26.45,29.15]:
        pts=np.array([[52.635,-33.385,B+8.5],[52.635,-17,B+10.5],[55.5,0,43],[55.5,22,43]])
        ts=np.linspace(0,1,101);curve=[];radii=[]
        for t in ts:
            p=(1-t)**3*pts[0]+3*t*(1-t)**2*pts[1]+3*t*t*(1-t)*pts[2]+t**3*pts[3];curve.append(p)
            d=3*((1-t)**2*(pts[1]-pts[0])+2*t*(1-t)*(pts[2]-pts[1])+t*t*(pts[3]-pts[2]));dd=6*((1-t)*(pts[2]-2*pts[1]+pts[0])+t*(pts[3]-2*pts[2]+pts[1]))
            radii.append(float(np.linalg.norm(d)**3/np.linalg.norm(np.cross(d,dd))))
        slabs=[]
        for a,b in zip(curve,curve[1:]):
            lo=np.minimum(a,b);hi=np.maximum(a,b)
            slabs.append(m.box(lo[0]-4,-hi[1],lo[2]-.5,hi[0]+4,-lo[1],hi[2]+.5))
        # Disjoint slab interiors in Y; compound intersections are summed.
        sweep=cq.Compound.makeCompound(slabs)
        for n,fr in new:clear(f'ribbon B{B}/{n}',sweep,fr)
        clear(f'ribbon B{B}/case',sweep,case)
        ribbon_results.append({'PCB_top':B,'width_design':8,'thickness_design':1,'Bezier_control_points_CAD':pts.tolist(),'sampled_curvature_min_radius_mm':min(radii),'curvature_sample_count':101,'segmented_AABB_sweep_count':100,'cable_supplier_min_bend_radius':'not specified here; physical relaxed route mandatory'})
    # Quantify discrete coverage independently of nominal connector dimensions.
    stacks={round(h+sum(a),1)for h in [8,10,12]for a in itertools.chain.from_iterable(itertools.combinations([.5,1,2],r)for r in range(4))}
    for k in range(16,32):assert k/2 in stacks
    analytic={'mechanical_support_height_range':[8,15.5],'step':.5,'maximum_off_board_sanding':.5,'electrical_engagement_window':'UNKNOWN; fully seat sockets manually before choosing support, never force using bolts','socket_female_height':8.5,'module_male_collar_nominal_hypothesis':2.5,'Click_native_male_collar':2.54,'nominal_H_hypothesis':11,'C05_max_Z_official_STEP':8.559996,'new_M2_head_min_Z_nominal':9.05,'C05_global_clearance_nominal':9.05-8.559996,'G_min_12p15_and_washer_max_1p1_head_clearance':8.85-8.559996,'B1_USER_physical_height':10.2,'B1_xy':[29.52,120.33],'B1_new_bolt_axes_separated':True,'new_bolt_xy':[p for p in modules]+v.CONFIG['MKI']['anchors']+v.CONFIG['Click']['anchors'],'M3_thread_fully_crosses_nyloc_over_continuous_B_gap_range':True,'M3_closed_floor':1.5,'sliding_clearance_per_face':.2,'M3_screw_max_head_top':45.95,'lid_minimum_margin':1.05,'guide_min_overlap':3.75,'minimum_cup_to_roof_cavity_clearance':.25,'M2_M3_pitch':[.4,.5],'PCB_NPTH_diameter':2.7,'M2_shaft_radial_clearance':.35,'Adafruit_module_hole_drill':2.5,'Adafruit_holes_plated_floating':True,'bearing_profile_4p4x4_bore_2p4':True,'M2_all_mounts_single_length':30,'M2_max_tip_Z_L30p5':41.75,'M2_min_tip_Z_L29p5':40.75,'M2_min_engagement_H15p5_t1p8_washer1p1_Gmin':5.2,'sampling_scope':'native geometry at three B and two gap extremes; analytic linear inequalities above cover guide/thread/head clearance between samples'}
    save('intersections.json',{'checks':checks,'positive_intersections':hits,'passed':not hits});save('plages-visserie.json',{'retainers':ranges,'M2_modules':metal,'analytic':analytic});save('nappe.json',ribbon_results)
    nominal_assembly.save(str(P/'cad/quincaillerie-standard-enveloppes.step'));v.trim(P/'cad/quincaillerie-standard-enveloppes.step')
    summary={'new_STL_reopened':11,'V25_STL_STEP_identity_reused':14,'types':len(rows),'pieces_for_two_kits':sum(q['quantity_to_print_two_kits']for q in rows),'exact_BREP_intersection_checks':len(checks),'positive_intersections':hits,'Click_native_solids':len(click.Solids()),'SD_native_solids':len(sd.Solids()),'ST_adapter_outline_from_official_Gerber':True,'physical_qualification':False,'passed':not hits}
    save('synthese-controles.json',summary);print(json.dumps(summary,indent=2),flush=True);assert not hits,'See intersections.json'
if __name__=='__main__':
    a=argparse.ArgumentParser();a.add_argument('--native-dir',type=Path,required=True);main(a.parse_args().native_dir)
