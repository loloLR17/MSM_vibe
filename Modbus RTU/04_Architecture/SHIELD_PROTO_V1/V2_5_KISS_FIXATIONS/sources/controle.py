"""Targeted checks for translated saddle and ordinary M3 metal hardware.

All unmodified print meshes are inherited by exact hashes from evaluated V24.
Whole PCB/native C05 bounding boxes are conservative electrical exclusion zones.
"""
from pathlib import Path
import json,math,hashlib
import cadquery as cq
import trimesh
from shapely.geometry import MultiPoint
import revision as v
P=v.P;V=v.V;m=v.m
def save(n,d):(P/'preuves'/n).write_text(json.dumps(d,indent=2,ensure_ascii=False)+'\n')
def bb(s):
    b=s.BoundingBox();return[b.xmin,b.xmax,b.ymin,b.ymax,b.zmin,b.zmax]
def overlap(a,b):return all(min(a[i+1],b[i+1])-max(a[i],b[i])>1e-8 for i in [0,2,4])
def gap(a,b):return math.sqrt(sum(max(a[i]-b[i+1],b[i]-a[i+1],0)**2 for i in [0,2,4]))
def nut(p,u,a):
    s=cq.Workplane('XY').polygon(6,5.5/(math.sqrt(3)/2)).extrude(2.4).val()
    if u==(0,0,-1):s=s.rotate((0,0,0),(1,0,0),180)
    elif u==(0,-1,0):s=s.rotate((0,0,0),(1,0,0),90)
    elif u==(0,1,0):s=s.rotate((0,0,0),(1,0,0),-90)
    elif u==(1,0,0):s=s.rotate((0,0,0),(0,1,0),90)
    return s.translate(tuple(p[k]+a*u[k]for k in range(3)))
def mounts():
    rows=[]
    for x,y in m.C['lid_screw_centres']:rows.append((f'lid_{x}_{y}',(x,-y,50),(0,0,-1),5.4))
    for x,y in m.C['case_keeper_pivots']:rows.append((f'keeper_{x}_{y}',(x,-y,4.4),(0,0,-1),5.4))
    for x in [127.5,149.5]:rows.append((f'tool_{x}',(x,-92.5,50),(0,0,-1),3))
    for y in [82,101]:rows.append((f'RS_{y}',(154,-y,30),(0,0,-1),7.4))
    for x in [30.5,85]:rows.append((f'window_{x}',(x,18.2,20),(0,-1,0),5.9))
    for x in [42.5,64.5]:rows.append((f'ribbon_{x}',(x,18.2,38),(0,-1,0),7.7))
    rows.extend([('USER',(45,-146.8,27),(0,1,0),5),('SDdoor',(7.2,-96,43),(1,0,0),5)])
    assert len(rows)==17;return rows
def main():
    rows=json.loads((P/'preuves/pieces.json').read_text());inherited=json.loads((V/'preuves/relecture-STEP-STL.json').read_text());proof={q['id']:q for q in inherited}
    for q in rows:
        old=next(x for x in json.loads((V/'preuves/pieces.json').read_text())if x['id']==q['id'])
        if q['id']!='01_base':assert (P/q['STL']).read_bytes()==(V/old['STL']).read_bytes()and proof[q['id']]['passed']
        if q['id']not in ['10_serre_nappe','01_base']:assert(P/q['STEP']).read_bytes()==(V/old['STEP']).read_bytes()
    old=cq.importers.importStep(str(V/'cad/10_serre_nappe.step')).val();new=cq.importers.importStep(str(P/'cad/10_serre_nappe.step')).val()
    expected=old.translate((0,2.5,0));diff=abs(new.cut(expected).Volume())+abs(expected.cut(new).Volume())
    assert diff<1e-6 and new.isValid()and len(new.Solids())==1
    mesh=trimesh.load_mesh(P/'stl/10_serre_nappe.stl',process=True)
    assert mesh.is_watertight and mesh.is_volume and len(mesh.split())==1
    ob=cq.importers.importStep(str(V/'cad/01_base.step')).val();nb=cq.importers.importStep(str(P/'cad/01_base.step')).val();be=v.base_correction(ob)
    for q in rows:
        if q['id']=='01_base':q['volume_mm3']=nb.Volume()
    save('pieces.json',rows)
    base_difference=abs(nb.cut(be).Volume())+abs(be.cut(nb).Volume());added=abs(nb.cut(ob).Volume())
    bm=trimesh.load_mesh(P/'stl/01_base.stl',process=True)
    assert base_difference<1e-6 and added<1e-6 and nb.isValid()and len(nb.Solids())==1
    assert bm.is_watertight and bm.is_winding_consistent and bm.is_volume and len(bm.split())==1 and abs(bm.volume-nb.Volume())/nb.Volume()<.01
    save('reprise-geometrique.json',{'unchanged_STL_exact_V24':13,'unchanged_individual_STEP_exact_V24':12,'changed_STEP':['01_base','10_serre_nappe'],'translation_saddle_CAD_mm':[0,2.5,0],'saddle_symmetric_difference_to_expected_mm3':diff,'base_symmetric_difference_to_expected_mm3':base_difference,'base_added_volume_mm3':added,'base_removed_volume_mm3':ob.Volume()-nb.Volume(),'modified_meshes_valid':True,'base_mesh_relative_volume_error':abs(bm.volume-nb.Volume())/nb.Volume(),'C05_reuse_proof':'Base only loses material (verified subset); cannot introduce collision relative to V24; unchanged D126/anchors/G','old_validation_reused':'V2_4_IMPRESSION_3D/preuves/relecture-STEP-STL.json; byte equality checked; no redundant mesh tests','source_V24_code_commit':'eb322dec649c36645edbbf7d2881e95ee0ea2df8'})
    parts=[(q['id'],cq.importers.importStep(str(P/q['STEP'])).val())for q in rows if q['id']not in ['13_D126','15_retenue_Nucleo']]
    # Candidate bridges / module envelopes remain conservative obstacles even
    # though no unqualified module retainer is included in the printer pack.
    refs=[('shield',m.box(20,17,12.25,110,117,13.85)),('FRAM',m.box(42.46,56.46,24.85,67.86,74.24,26.45)),('SD',m.box(20.18,83.14,24.85,43.04,108.54,30.0)),('MKI_PROVISIONAL',m.box(42.46,21.46,24.85,62.78,54.48,34.85)),('Click_PROVISIONAL',m.box(83.41,78.73,24.85,140.56,104.13,34.94))]
    meta=json.loads((P.parent/'V2_0_EMBOITEMENT/preuves/C05-collisions-G12p25.json').read_text());Tx,Ty=meta['Tx_Ty'];native=[]
    for q in meta['source_geometry']:
        b=q['bbox_ST'];native.append([b[0]+Tx,b[1]+Tx,b[2]-Ty,b[3]-Ty,b[4],b[5]])
    partboxes={n:bb(s)for n,s in parts};refboxes={n:bb(s)for n,s in refs}
    hits=[];distances=[];nutfit=[];asm=cq.Assembly(name='V25_M3x12_DESIGN_ENVELOPES_NOT_VENDOR_MODELS')
    for name,p,u,a in mounts():
        n=nut(p,u,a);assert n.isValid()
        nutfit.append({'mount':name,'nut_entry_from_head':a,'nut_exit':a+2.4,'minimum_thread_emergence_at_L11p5':11.5-a-2.4})
        assert 11.5-a-2.4>=1.0
        for L in [11.5,12,12.5]:
            bolt=cq.Solid.makeCylinder(1.5,L,cq.Vector(*p),cq.Vector(*u)).fuse(cq.Solid.makeCylinder(3,3.2,cq.Vector(*p),cq.Vector(*[-x for x in u])))
            for kind,s in [('bolt',bolt),('nut',n)]:
                b=bb(s)
                for pn,ps in parts:
                    if overlap(b,partboxes[pn]):
                        vol=abs(s.intersect(ps).Volume())
                        if vol>1e-6:hits.append({'mount':name,'kind':kind,'L':L,'part':pn,'mm3':vol})
                for rn,rs in refs:
                    # Full PCB/component boxes must be strictly separated.
                    d=gap(b,refboxes[rn]);distances.append({'mount':name,'kind':kind,'L':L,'reference':rn,'lower_bound_mm':d})
                    if d<=0:hits.append({'mount':name,'kind':kind,'L':L,'reference':rn,'error':'conservative electrical envelopes overlap'})
                dn=min(gap(b,x)for x in native);distances.append({'mount':name,'kind':kind,'L':L,'reference':'1060_ST_boxes','lower_bound_mm':dn})
                if dn<=0:hits.append({'mount':name,'kind':kind,'L':L,'error':'ST source box overlaps; exact analysis required'})
            if L==12:asm.add(bolt,name=name+'_bolt');asm.add(n,name=name+'_nut')
    # The sole changed printed part still clears every other global part and
    # the old candidate bridges (and it stays far above the LED aperture).
    for pn,ps in parts+[(n,cq.importers.importStep(str(V/'cad'/(n+'.step'))).val())for n in ['16_etrier_MKI','17_etrier_Click']]:
        if pn=='10_serre_nappe':continue
        if overlap(bb(new),bb(ps)):
            vol=abs(new.intersect(ps).Volume())
            if vol>1e-6:hits.append({'saddle_collision':pn,'mm3':vol})
    for rn,rs in refs:
        if gap(bb(new),bb(rs))<=0:hits.append({'saddle_reference_box_overlap':rn})
    asm.save(str(P/'cad/quincaillerie-M3x12-enveloppes.step'));v.trim(P/'cad/quincaillerie-M3x12-enveloppes.step')
    # Verify actual captive pockets against the maximum ordinary nut prism.
    # All are covered by the explicit collision tests above, not only dimension arithmetic.
    loading=[];base=next(s for name,s in parts if name=='01_base')
    for name,p,u,a in mounts():
        if not name.startswith(('lid_','keeper_','RS_')):continue
        n=nut(p,u,a)
        pts=[(x.X+shift,x.Y)for x in n.Vertices()for shift in [-10,0]]
        hull=MultiPoint(pts).convex_hull
        swept=cq.Workplane('XY').polyline(list(hull.exterior.coords)[:-1]).close().extrude(2.4).val().translate((0,0,n.BoundingBox().zmin))
        vol=abs(swept.intersect(base).Volume())
        if vol>1e-6:loading.append({'mount':name,'continuous_sweep_X_mm':[-10,0],'mm3':vol})
    if loading:hits.append({'captive_nut_loading_collisions':loading})
    summary={'mounts_per_kit':17,'lengths_design_acceptance_mm':[11.5,12,12.5],'standard_nominal':'M3x12 DIN912 / ISO4762; head typical5.5x3','head_test_envelope_d_h_mm':[6,3.2],'nut_standard':'M3 DIN934 similar ISO4032, AF5.5 Hmax2.4','existing_pocket_AF_height_mm':[5.9,2.8],'pocket_clearance_per_flat_mm':.2,'pocket_height_clearance_mm':.4,'all_nuts_fully_engaged':True,'minimum_thread_emergence_mm':min(q['minimum_thread_emergence_at_L11p5']for q in nutfit),'positive_intersections':hits,'minimum_electrical_box_clearance_mm':min(d['lower_bound_mm']for d in distances),'PCB_copper_contact':False if not hits else 'UNRESOLVED','isolation_scope':'No structural metal contacts even full PCB/component bounding boxes, hence no conductive bridge through this hardware; not an insulation standard/voltage qualification','passed':not hits}
    save('M3-controle.json',summary);save('M3-distances.json',{'bounds':distances,'nut_engagement':nutfit,'nut_loading_bare_case_before_boards':loading})
    print(json.dumps(summary,indent=2),flush=True);assert not hits
if __name__=='__main__':main()
