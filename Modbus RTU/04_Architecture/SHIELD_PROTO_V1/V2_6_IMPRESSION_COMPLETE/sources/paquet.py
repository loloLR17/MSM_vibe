"""One complete prototype print ZIP; all entries reopened, CRC and SHA checked."""
from pathlib import Path
import csv,hashlib,json,zipfile,io
P=Path(__file__).resolve().parents[1]
def sha(b):return hashlib.sha256(b).hexdigest()
def read(n):return json.loads((P/'preuves'/n).read_text())
def save(n,d):(P/'preuves'/n).write_text(json.dumps(d,indent=2,ensure_ascii=False)+'\n')
def main():
    proof=read('synthese-controles.json');assert proof['passed']and not proof['positive_intersections']
    rows=read('pieces.json');assert len(rows)==25 and sum(q['quantity_to_print_two_kits']for q in rows)==142
    by={q['id']:q for q in rows}
    assert by['30_etrier_MKI']['quantity_to_print_two_kits']==by['31_etrier_Click']['quantity_to_print_two_kits']==2
    assert all(by[n]['quantity_to_print_two_kits']>=8 for n in ['20_appui_H08','21_appui_H10','22_appui_H12'])
    qty=list(csv.DictReader(io.StringIO((P/'QUANTITES.csv').read_text()),delimiter=';'))
    assert {q['STL']:int(q['quantite_deux_prototypes'])for q in qty}=={q['id']+'.stl':q['quantity_to_print_two_kits']for q in rows}
    totalvol=sum(q['volume_mm3']*q['quantity_to_print_two_kits']for q in rows)
    mass={'parts_including_variants_and_spares':142,'solid_CAD_volume_mm3':totalvol,'PETG_density_scenario_g_cm3':1.27,'solid_mass_g':totalvol*.00127,'basis':'Dense CAD volume; actual slicer infill/support/brim/purge not evaluated; not a purchasing or filament consumption guarantee. No actual print performed.','actual_slicer_mass':None,'print_time_estimate':None}
    save('masse.json',mass)
    payload={
        'LISEZ_MOI_IMPRIMEUR.md':(P/'IMPRIMEUR.md').read_bytes(),
        'MONTAGE_A_FROID.md':(P/'MONTAGE_A_FROID.md').read_bytes(),
        'QUANTITES.csv':(P/'QUANTITES.csv').read_bytes(),
        'BOM_QUINCAILLERIE.csv':(P/'BOM_QUINCAILLERIE.csv').read_bytes(),
        'ASSEMBLAGE_AJUSTEMENT_V26.pdf':(P/'vues/ASSEMBLAGE_AJUSTEMENT_V26.pdf').read_bytes(),
        'MASSE.json':(P/'preuves/masse.json').read_bytes()}
    for q in rows:
        b=(P/q['STL']).read_bytes();assert sha(b)==q['STL_sha256'];payload['STL/'+q['id']+'.stl']=b
    manifest={'mission_id':'TR2-20261105-SHIELD-V26-IMPRESSION-COMPLETE-SANS-RECEPTION','release':'COMPLET_IMPRIMABLE_AVEC_AJUSTEMENT_A_FROID_POUR_REVUE','physical_qualification':False,'two_prototypes':True,'types':25,'parts':142,'included_retainers':{'MKI':2,'Click':2},'missing_printed_parts':[],'fixed_hardware':'M3x12 metal, M2x30 metal; Ajile M2x20 nylon exclusively D126','files':{k:sha(b)for k,b in sorted(payload.items())}}
    payload['MANIFESTE.json']=(json.dumps(manifest,indent=2,ensure_ascii=False)+'\n').encode()
    f=P/'pack_imprimeur/TR2_V26_COMPLET_2_PROTOTYPES.zip'
    with zipfile.ZipFile(f,'w',zipfile.ZIP_DEFLATED,compresslevel=9)as z:
        for n,b in sorted(payload.items()):
            zi=zipfile.ZipInfo(n,(2026,11,5,0,0,0));zi.compress_type=zipfile.ZIP_DEFLATED;zi.external_attr=0o644<<16;z.writestr(zi,b)
    with zipfile.ZipFile(f)as z:
        assert z.testzip()is None and set(z.namelist())==set(payload)
        for n,b in payload.items():assert z.read(n)==b
        assert len([n for n in z.namelist()if n.endswith('.stl')])==25
        assert not any(n.endswith(('.step','.brep','.py','.exe'))for n in z.namelist())
    repo=P.parents[3];baseline=read('conservation-initiale.json')['files']
    bad=[n for n,h in baseline.items()if not(repo/n).is_file()or sha((repo/n).read_bytes())!=h];assert not bad,bad
    save('conservation-finale.json',{'preexisting_files_compared':len(baseline),'mismatches':bad,'PCB_CAM_firmware_historical_shield_files_unchanged':True})
    save('pack-manifeste.json',{'ZIP':str(f.relative_to(P)),'sha256':sha(f.read_bytes()),'bytes':f.stat().st_size,'entries':len(payload),'complete_CRC_readback':True,'all_members_bytes_compared':True,'remote_readback_record':'Reported separately after normal push in DERNIER_RAPPORT.md',**manifest})
    print(json.dumps({'ZIP':str(f),'sha256':sha(f.read_bytes()),'entries':len(payload),'types':25,'pieces':142,'dense_scenario_mass_g':mass['solid_mass_g'],'preserved_baseline_files':len(baseline)},indent=2),flush=True)
if __name__=='__main__':main()
