"""Create/reopen the single partial printer ZIP; preserve historical deliverables."""
from pathlib import Path
import json,csv,io,hashlib,zipfile,math
P=Path(__file__).resolve().parents[1]
def sha(b):return hashlib.sha256(b).hexdigest()
def read(n):return json.loads((P/'preuves'/n).read_text())
def save(n,d):(P/'preuves'/n).write_text(json.dumps(d,indent=2,ensure_ascii=False)+'\n')
def main():
    coords=json.loads((P/'sources/geometrie-reference.json').read_text());params=json.loads((P/'sources/parametres.json').read_text())
    pcb=P.parent/'V2_3_ARNACO/cad/TR2_Shield_Proto_V1.kicad_pcb'
    assert sha(pcb.read_bytes())==coords['PCB_V23_SHA256']
    matches=[]
    for key in ['anchors','module_supports_FRAM','module_supports_SD','MKI_retainer_anchors','Click_retainer_anchors']:
        for x,y in params[key]:
            found=[q for q in coords['pads']if q['npth']and abs(q['x']-x)<1e-6 and abs(q['y']-y)<1e-6]
            assert len(found)==1,(key,x,y)
            matches.append({'group':key,'x':x,'y':y,'drill':found[0]['drill'],'V23_ref':found[0]['ref']})
    assert params['G']==12.25 and params['shield_top']==13.85
    save('axes-V23.json',{'PCB_V23_SHA256':coords['PCB_V23_SHA256'],'anchors_matched':matches,'G':12.25,'shield_top':13.85,'same_electrical_files':True})
    check=read('synthese-controle.json');services=read('services.json')
    assert check['mesh_passed']==check['meshes_total']==17 and check['logo_passed']and check['D126_identity']
    assert not check['own_hits']and not check['reference_hits']and not check['C05_population_hits']and services['passed']
    assert read('relief-ARNACO-relu.json')['passed']
    rows=read('pieces.json');ready=[r for r in rows if r['status']=='PRET_A_IMPRIMER'];excluded=[r for r in rows if r['status']!='PRET_A_IMPRIMER']
    assert len(ready)==15 and len(excluded)==2
    out=io.StringIO();writer=csv.writer(out,delimiter=';',lineterminator='\n')
    writer.writerow(['STL','quantite_deux_kits','utiles','reserves','dimensions_X_Y_Z_mm','orientation_deja_appliquee','note'])
    for r in ready:writer.writerow([Path(r['STL']).name,r['quantity_to_print_two_kits'],r['useful_two_kits'],r['spares'],' x '.join(f'{x:.3f}'for x in r['dimensions_print_mm']),r['orientation'],r['note']])
    qty=out.getvalue().encode();(P/'QUANTITES.csv').write_bytes(qty)
    volume=sum(r['quantity_to_print_two_kits']*r['volume_mm3']for r in ready)
    mass={'scope':'15 authorized types for two prototypes, printed spares included, excluded bridges absent',
      'printed_parts':sum(r['quantity_to_print_two_kits']for r in ready),
      'solid_CAD_volume_mm3':volume,'density_scenario_g_cm3':1.27,
      'solid_CAD_mass_g':volume*.00127,'filament_diameter_scenario_mm':1.75,
      'solid_filament_length_m':volume/(math.pi*(1.75/2)**2)/1000,
      'budget_allowance_fraction_not_measured':.2,'solid_mass_plus_allowance_g':volume*.00127*1.2,
      'time':'non estimé','energy':'non estimée','actual_sliced_mass':None,
      'density_source':'https://prusament.com/wp-content/uploads/2022/10/PETG_Prusament_TDS_2021_10_EN.pdf',
      'limits':'Dense CAD scenario, no slicer run, no mechanical rating. Allowance is budget only, not a guarantee against failed prints.'}
    save('masse.json',mass)
    payload={'README_IMPRIMEUR.md':(P/'IMPRIMEUR.md').read_bytes(),'QUANTITES.csv':qty,
      'BOM_NON_IMPRIMEE.csv':(P/'BOM_NON_IMPRIMEE.csv').read_bytes(),
      'MASSE.json':(P/'preuves/masse.json').read_bytes(),'ASSEMBLAGE_ET_COUPES.pdf':(P/'vues/ASSEMBLAGE_ET_COUPES.pdf').read_bytes(),
      'ASSEMBLAGE.md':(P/'ASSEMBLAGE.md').read_bytes(),'RESERVES.md':(P/'RESERVES.md').read_bytes()}
    for r in ready:payload['STL/'+Path(r['STL']).name]=(P/r['STL']).read_bytes()
    manifest={'mission_id':'TR2-20261105-SHIELD-V24-IMPRESSION-3D','status':'PARTIEL_CONTROLE_NUMERIQUE_PAS_QUALIFICATION_PHYSIQUE','types_included':len(ready),'pieces_to_print':mass['printed_parts'],
      'excluded':[{'id':r['id'],'quantity_missing':r['useful_two_kits'],'reason':r['note']}for r in excluded],
      'files':{n:sha(b)for n,b in sorted(payload.items())}}
    payload['MANIFESTE.json']=(json.dumps(manifest,indent=2,ensure_ascii=False)+'\n').encode()
    target=P/'pack_imprimeur/TR2_V24_2_PROTOTYPES_PARTIEL.zip'
    with zipfile.ZipFile(target,'w',zipfile.ZIP_DEFLATED,compresslevel=9)as z:
        for name,b in sorted(payload.items()):
            zi=zipfile.ZipInfo(name,(2026,11,5,0,0,0));zi.compress_type=zipfile.ZIP_DEFLATED;zi.external_attr=0o644<<16;z.writestr(zi,b)
    with zipfile.ZipFile(target)as z:
        assert z.testzip()is None and set(z.namelist())==set(payload)
        for name,b in payload.items():assert z.read(name)==b
        stls=[n for n in z.namelist()if n.endswith('.stl')]
        assert len(stls)==15 and not any(n.startswith(('STL/16_','STL/17_'))for n in stls)
        assert not any(n.endswith(('.step','.brep','.3mf'))or'DRAFT'in n for n in z.namelist())
    # Exactly the pre-existing tracked shield files remain byte-identical.
    repo=P.parents[3];old=read('conservation-initiale.json');bad=[]
    for rel,h in old['files'].items():
        f=repo/rel
        if not f.is_file()or sha(f.read_bytes())!=h:bad.append(rel)
    assert not bad,bad
    save('conservation-finale.json',{'compared_preexisting_shield_files':len(old['files']),'mismatches':bad,'initial_head':old['initial_head'],'scope':'all pre-existing tracked shield artifacts, including V23/V19 PCB and CAM, preserved byte-for-byte'})
    save('pack-manifeste.json',{'zip':str(target.relative_to(P)),'sha256':sha(target.read_bytes()),'bytes':target.stat().st_size,'entries':len(payload),'readback_all_bytes_identical':True,**manifest})
    print(json.dumps({'ZIP':str(target),'SHA256':sha(target.read_bytes()),'entries':len(payload),'types':len(ready),'pieces':mass['printed_parts'],'solid_mass_g':mass['solid_CAD_mass_g'],'preserved':len(old['files'])},indent=2))
if __name__=='__main__':main()
