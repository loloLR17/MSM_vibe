"""Single explicit review ZIP, exact part counts and preserved historical files."""
from pathlib import Path
import json,hashlib,zipfile,csv,io
P=Path(__file__).resolve().parents[1]
def sha(b):return hashlib.sha256(b).hexdigest()
def read(n):return json.loads((P/'preuves'/n).read_text())
def save(n,x):(P/'preuves'/n).write_text(json.dumps(x,indent=2,ensure_ascii=False)+'\n')
def main():
    assert read('M3-controle.json')['passed']and read('bras-pivots.json')['passed'];proof=read('reprise-geometrique.json')
    assert proof['unchanged_STL_exact_V24']==13 and proof['base_added_volume_mm3']<1e-6
    rows=read('pieces.json');assert len(rows)==14
    assert not any(q['id'].startswith(('14_','16_','17_'))for q in rows)
    assert sum(q['quantity_to_print_two_kits']for q in rows)==40
    qty=io.StringIO();w=csv.writer(qty,delimiter=';',lineterminator='\n');w.writerow(['STL','quantite_deux_prototypes','utiles','reserve','fonction','identique_V24'])
    for q in rows:w.writerow([Path(q['STL']).name,q['quantity_to_print_two_kits'],q['useful_two_kits'],q['spares'],'A_D126'if q['id']=='13_D126'else'C_COFFRET',q['STL_identical_to_V24']])
    (P/'QUANTITES.csv').write_text(qty.getvalue())
    volume=sum(q['volume_mm3']*q['quantity_to_print_two_kits']for q in rows)
    mass={'printed_parts':40,'solid_volume_mm3':volume,'PETG_density_scenario_g_cm3':1.27,'solid_mass_g':volume*.00127,'slicer_mass':None,'time':'non estimé','energy':'non estimée','basis':'V24 density scenario reused, not measured filament. No support/brim/purge included. No guarantee against failed prints.'};save('masse.json',mass)
    payload={'README_IMPRIMEUR.md':(P/'IMPRIMEUR.md').read_bytes(),'QUANTITES.csv':(P/'QUANTITES.csv').read_bytes(),'BOM_QUINCAILLERIE.csv':(P/'BOM_QUINCAILLERIE.csv').read_bytes(),'RESERVES.md':(P/'RESERVES.md').read_bytes(),'SCHEMAS_MONTAGE_V25.pdf':(P/'vues/SCHEMAS_MONTAGE_V25.pdf').read_bytes(),'MASSE.json':(P/'preuves/masse.json').read_bytes()}
    for q in rows:
        b=(P/q['STL']).read_bytes();assert sha(b)==q['STL_sha256'];payload['STL/'+Path(q['STL']).name]=b
    manifest={'mission_id':'TR2-20261105-SHIELD-V25-KISS-FIXATIONS','release':'PARTIEL_POUR_REVUE_PAS_LOT_COMPLET','printed_types':14,'parts':40,'missing':['FRAM/SD compression supports at actual H','2MKI and2Click retainers'],'structural_hardware':'M3x12 metal +DIN934 M3; 34each useful,40with reserves','D126':'Ajile nylon reserved exclusively;6bolts12nuts useful,8/16purchased','files':{k:sha(b)for k,b in sorted(payload.items())}}
    payload['MANIFESTE.json']=(json.dumps(manifest,indent=2,ensure_ascii=False)+'\n').encode()
    f=P/'pack_imprimeur/TR2_V25_KISS_PARTIEL_REVUE.zip'
    with zipfile.ZipFile(f,'w',zipfile.ZIP_DEFLATED,compresslevel=9)as z:
        for n,b in sorted(payload.items()):
            info=zipfile.ZipInfo(n,(2026,11,5,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED;info.external_attr=0o644<<16;z.writestr(info,b)
    with zipfile.ZipFile(f)as z:
        assert z.testzip()is None and set(z.namelist())==set(payload)
        for n,b in payload.items():assert z.read(n)==b
        assert len([n for n in z.namelist()if n.endswith('.stl')])==14
        assert not any(n.endswith(('.step','.brep','.py','.exe'))for n in z.namelist())
    old=read('conservation-initiale.json');repo=P.parents[3]
    bad=[name for name,h in old['files'].items()if not(repo/name).is_file()or sha((repo/name).read_bytes())!=h];assert not bad,bad
    save('conservation-finale.json',{'preexisting_shield_files_compared':len(old['files']),'mismatches':bad,'all_V24_and_PCB_CAM_unchanged':True})
    save('pack-manifeste.json',{'ZIP':str(f.relative_to(P)),'sha256':sha(f.read_bytes()),'bytes':f.stat().st_size,'entries':len(payload),'all_remote_independent_verification_pending':True,'readback_all_members_identical':True,**manifest})
    print(json.dumps({'ZIP':str(f),'sha256':sha(f.read_bytes()),'entries':len(payload),'types':14,'pieces':40,'solid_mass_g':mass['solid_mass_g'],'preserved_files':len(old['files'])},indent=2))
if __name__=='__main__':main()
