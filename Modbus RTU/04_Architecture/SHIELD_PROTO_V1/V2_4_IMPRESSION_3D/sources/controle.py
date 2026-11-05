"""Reopen exported geometry, check meshes and bounded nominal assembly.

Run with the documented CAD Python environment. Manufacturer C05 BREP stays local.
No FDM, strength, optical-emission or physical fit qualification is inferred.
"""
import json, hashlib, math, sys
from pathlib import Path
import cadquery as cq
import trimesh
import modele as m

P=m.P; R=P.parent
def save(n,d): (P/'preuves'/n).write_text(json.dumps(d,indent=2,ensure_ascii=False)+'\n')
def bounds(s):
    b=s.BoundingBox();return [b.xmin,b.xmax,b.ymin,b.ymax,b.zmin,b.zmax]
def overlap(a,b):
    return all(min(a[2*k+1],b[2*k+1])-max(a[2*k],b[2*k])>1e-8 for k in range(3))
def volume(a,b): return abs(a.intersect(b).Volume())

def main():
    rows=json.loads((P/'preuves/pieces.json').read_text()); reopened={}; mesh_checks=[]
    for row in rows:
        s=cq.importers.importStep(str(P/row['STEP'])).val();reopened[row['id']]=s
        mesh=trimesh.load_mesh(P/row['STL'],process=True)
        ratio=abs(mesh.volume-s.Volume())/s.Volume()
        check={'id':row['id'],'STEP_valid':s.isValid(),'STEP_solids':len(s.Solids()),
          'watertight':bool(mesh.is_watertight),'winding_consistent':bool(mesh.is_winding_consistent),
          'positive_mesh_volume':bool(mesh.is_volume),'mesh_components':len(mesh.split()),
          'mesh_volume_mm3':float(mesh.volume),'STEP_volume_mm3':s.Volume(),'relative_volume_error':ratio,
          'mesh_bounds_mm':mesh.bounds.tolist(),'triangles':len(mesh.faces),
          'density_scenario_1p27_solid_mass_g':s.Volume()*.00127,
          'quantity':row['quantity_to_print_two_kits'],'status':row['status']}
        expected=bounds(m.normalized(s,row['orientation']))
        expected_mesh_order=[[expected[0],expected[2],expected[4]],[expected[1],expected[3],expected[5]]]
        check['bounds_error_mm']=max(abs(mesh.bounds[j,k]-expected_mesh_order[j][k])for j in range(2)for k in range(3))
        check['export_chord_tolerance_mm']=.025 if row['id']=='13_D126'else .06
        check['passed']=all([check['STEP_valid'],check['STEP_solids']==1,check['watertight'],check['winding_consistent'],check['positive_mesh_volume'],check['mesh_components']==1,ratio<.01,check['bounds_error_mm']<=check['export_chord_tolerance_mm'],max(mesh.extents[:2])+10<=180,mesh.extents[2]<70])
        mesh_checks.append(check)
    save('relecture-STEP-STL.json',mesh_checks)
    lid=reopened['02_couvercle_ARNACO']; raised=lid.intersect(m.box(0,-200,50,200,200,52))
    logo=json.loads((P/'preuves/relief-ARNACO.json').read_text())
    logo.update({'reopened_STEP_valid':lid.isValid(),'reopened_lid_solids':len(lid.Solids()),
      'raised_glyphs_above_Z50':len(raised.Solids()),'raised_volume_mm3':raised.Volume(),
      'measured_top_Z':lid.BoundingBox().zmax,'measured_added_volume':lid.Volume()-logo['plain_lid_volume']})
    logo['passed']=len(raised.Solids())==6 and abs(raised.Volume()-logo['emboss_area_mm2'])<1e-5 and abs(lid.BoundingBox().zmax-51)<1e-7 and len(lid.Solids())==1
    save('relief-ARNACO-relu.json',logo)
    # Exact historical D126 profile is re-used, not a rerun of V21 nine tolerance cases.
    old=cq.importers.importStep(str(R/'V2_1_FIXATIONS/cad/D126-support.step')).val()
    new=reopened['13_D126']; residual=old.cut(new).Volume()+new.cut(old).Volume()
    save('D126-identite.json',{'symmetric_difference_mm3':residual,'old_bounds':bounds(old),'new_bounds':bounds(new),'same_profile':residual<1e-6,'prior_nine_cases':'V2_1_FIXATIONS/preuves/validation-finale.json; not repeated'})
    refs,_=m.build()
    # All own occurrences checked; candidate bridges are identified individually.
    inst=m.INST; own_hits=[]; ref_hits=[]
    for i,a in enumerate(inst):
        for b in inst[i+1:]:
            if overlap(bounds(a['shape']),bounds(b['shape'])):
                v=volume(a['shape'],b['shape'])
                if v>1e-6:own_hits.append({'a':a['label'],'b':b['label'],'mm3':v})
        for name,s in refs:
            if overlap(bounds(a['shape']),bounds(s)):
                v=volume(a['shape'],s)
                if v>1e-6:ref_hits.append({'part':a['label'],'reference':name,'mm3':v})
    save('assemblage-nominal.json',{'own_instances':len(inst),'reference_envelopes':len(refs),'own_positive_intersections':own_hits,'envelope_positive_intersections':ref_hits,'module_bottom_scenario':24.85,'scope':'nominal boxes are not exact module/plug hardware; no mechanical qualification'})
    # Native ST source: same cache/transform/population rule already traced in V20.
    meta=json.loads((R/'V2_0_EMBOITEMENT/preuves/C05-collisions-G12p25.json').read_text())
    cache=Path('/home/lolo/dev/msm/reports/TR2-shield-v20-20261105/C05-cache.brep')
    native=cq.Shape.importBrep(str(cache));Tx,Ty=meta['Tx_Ty'];solids=[s.translate((Tx,-Ty,0))for s in native.Solids()]
    solid_bounds=[bounds(s)for s in solids]
    excluded=[r['index']for r in meta['source_geometry']if r['excluded_from_actual_population_case']]
    # Superset occupancy cells avoid expensive Boolean tests against the empty
    # chamber. Prove that their union covers the actual exported base first.
    cells=[m.box(8,-20,-14,166,146,-11),m.box(8,-20,-11,166,-17,47),m.box(8,143,-11,166,146,47),m.box(8,-20,-11,11,146,47),m.box(163,-20,-11,166,146,47)]
    for x,y in m.C['lid_screw_centres']:cells.append(m.box(x-5,y-5,-11,x+5,y+5,47))
    for x,y in m.C['anchors']:cells.append(m.box(x-5,y-5,-11,x+5,y+5,-1.6))
    for x,y in m.C['case_keeper_pivots']:cells.append(m.box(x-4.5,y-4.5,-11,x+4.5,y+4.5,1.4))
    for x,y in [(154,82),(154,101)]:cells.append(m.box(x-5,y-5,-11,x+5,y+5,25))
    for u,U,z,wall in [(27,91,-7,'n'),(20,90,-7,'s'),(78,114,19,'w'),(75,110,21,'e')]:
        for a,b in [(u-4,u),(U,U+4)]:
            cells.append(m.box(a,-20,z-2,b,-12,47)if wall=='n'else m.box(a,138,z-2,b,146,47)if wall=='s'else m.box(8,a,z-2,16,b,47)if wall=='w'else m.box(158,a,z-2,166,b,47))
    remainder=reopened['01_base']
    for s in cells: remainder=remainder.cut(s)
    uncovered=abs(remainder.Volume());assert uncovered<1e-6,uncovered
    print('Base coverage proved',uncovered,flush=True)
    occupancy=[bounds(s)for s in cells]
    hits=[]; contacts=[]; boolean_count=0
    for a in inst:
        print('C05 check',a['label'],flush=True)
        ab=bounds(a['shape'])
        for j,s in enumerate(solids):
            sb=solid_bounds[j]
            if a['part']=='01_base'and not any(overlap(bb,sb)for bb in occupancy):continue
            if overlap(ab,sb):
                boolean_count+=1
                common=a['shape'].intersect(s);v=abs(common.Volume())
                if v>1e-6:
                    bb=bounds(common)
                    row={'part':a['label'],'ST_index':j,'mm3':v,'intersection_bounds':bb,'full_Morpho_model_excluded_in_population_scenario':j in excluded}
                    if j==0 and bb[5]-bb[4]<.00001 and a['part']=='01_base': contacts.append(row)
                    else:hits.append(row)
    save('C05-nominal.json',{'source_sha256_from_V20':meta['ST_source_sha256'],'cache_sha256':hashlib.sha256(cache.read_bytes()).hexdigest(),'source_solids':len(solids),'base_occupancy_uncovered_volume':uncovered,'Boolean_pairs_checked':boolean_count,'excluded_full_Morpho_indices':excluded,'full_source_positive_intersections':hits,'population_scenario_positive_intersections':[h for h in hits if h['ST_index']not in excluded],'intentional_bearing_numeric_contact':contacts,'floor_clearance_mm':-10.0076001-(-11),'scope':'full-source and documented populated-header scenario distinguished; not actual hardware qualification'})
    # Conservative finite ray/AABB checks: a clear ray through all bounding boxes
    # proves only this CAD ray, not all LED emission points or optical brightness.
    def blocked(p,q,bb):
        low,high=.01,.98
        for k in range(3):
            d=q[k]-p[k]
            if abs(d)<1e-12:
                if not bb[2*k]<p[k]<bb[2*k+1]:return False
            else:
                a=(bb[2*k]-p[k])/d;b=(bb[2*k+1]-p[k])/d
                low=max(low,min(a,b));high=min(high,max(a,b))
                if low>=high:return False
        return True
    obstacles=[(f'ST_{j}',solid_bounds[j])for j,s in enumerate(solids)if j not in excluded and solid_bounds[j][5]>0]
    obstacles +=[(n,bounds(s))for n,s in refs if n!='PC_window_not_printed']
    obstacles +=[(a['label'],bounds(a['shape']))for a in inst if a['part']not in ['01_base','03_cassette_nord','07_cadre_vitre']]
    ledpts=[(d['ref'],[d['xy_shield_mm'][0],-d['xy_shield_mm'][1],d['height_mm']+.1])for d in m.C['LEDs']]
    trials=[]
    for x in range(36,80,2):
        for z in [14,18,22,26]:
            p=[x,21,z];rays=[]
            for name,q in ledpts:
                hits0=[n for n,b in obstacles if blocked(p,q,b)]
                for yy in [20,18.2,15.8,14.3,12.3]:
                    t=(yy-p[1])/(q[1]-p[1]);xx=p[0]+t*(q[0]-p[0]);zz=p[2]+t*(q[2]-p[2])
                    if not(35.05<xx<79.95 and 13.05<zz<26.95):hits0.append('window_or_frame_aperture')
                rays.append({'LED':name,'CAD_target':q,'blockers_AABB':hits0})
            if all(not q['blockers_AABB']for q in rays):trials.append({'observer':p,'rays':rays})
    save('optique-bornage.json',{'clear_common_CAD_observers':trials,'target_definition':'ST footprint/body origin at modeled height+0.1, not measured emission point','method':'88 candidate observers; conservative AABB blockers; actual pane/brightness/eye positions unknown','optical_qualification':False})
    summary={'mesh_passed':sum(r['passed']for r in mesh_checks),'meshes_total':len(rows),'logo_passed':logo['passed'],'own_hits':own_hits,'reference_hits':ref_hits,'C05_population_hits':[h for h in hits if h['ST_index']not in excluded],'optical_clear_observers':len(trials),'D126_identity':residual<1e-6}
    save('synthese-controle.json',summary);print(json.dumps(summary,indent=2),flush=True)
    assert all(r['passed']for r in mesh_checks) and logo['passed'] and residual<1e-6
    assert not own_hits and not ref_hits and not summary['C05_population_hits']

if __name__=='__main__':main()
