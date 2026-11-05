"""Targeted V25 derivative. V24 originals are read only; unchanged STL copied.

The saddle moves 2.5mm and proved nut-loading obstructions are relieved locally.
Module supports require an actual mating-height measurement: excluded for now.
"""
from pathlib import Path
import importlib.util,json,shutil,hashlib
import cadquery as cq
P=Path(__file__).resolve().parents[1];R=P.parent;V=R/'V2_4_IMPRESSION_3D'
spec=importlib.util.spec_from_file_location('v24_model',V/'sources/modele.py')
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
EXCLUDED={'14_appui_module_reglable_M2','16_etrier_MKI','17_etrier_Click'}
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def base_correction(s):
    # Rail fused after original pocket creation refilled its edge (observed).
    s=s.cut(m.hexhole(13,141,41.8,5.9,2.8))
    # Bare-case nut loading, before fitting Nucleo: two west access windows.
    for y in [21.192131,97.39]:s=s.cut(m.box(4,y-3,-3.8,16,y+3,-1))
    # Two right lid pockets: extend their existing inward loading channels.
    for y in [-15,141]:s=s.cut(m.box(144,y-3,41.8,161,y+3,44.6))
    return s.clean()
def assembly():
    refs,logo=m.build()
    items=[]
    for row in m.INST:
        if row['part']in EXCLUDED:continue
        s=row['shape'].translate((0,2.5,0))if row['part']=='10_serre_nappe'else row['shape']
        if row['part']=='01_base':s=base_correction(s)
        items.append({**row,'shape':s})
    return items,refs
def trim(f):
    b=f.read_bytes();c=b'\n'.join(q.rstrip()for q in b.splitlines())+b'\n';assert b.split()==c.split();f.write_bytes(c)
def main():
    rows=json.loads((V/'preuves/pieces.json').read_text());items,refs=assembly();out=[]
    asm=cq.Assembly(name='V25_PRINTABLE_OWN_PARTS_ONLY')
    for row in items:asm.add(row['shape'],name=row['label'])
    asm.save(str(P/'cad/assemblage-imprimable.step'));trim(P/'cad/assemblage-imprimable.step')
    for row in rows:
        n=row['id']
        if n in EXCLUDED:continue
        dst=P/'cad'/(n+'.step');old=V/row['STEP']
        if n in ['10_serre_nappe','01_base']:
            s=cq.importers.importStep(str(old)).val()
            s=s.translate((0,2.5,0))if n=='10_serre_nappe'else base_correction(s)
            cq.exporters.export(s,str(dst));trim(dst)
        else:shutil.copyfile(old,dst)
        stl=P/'stl'/(n+'.stl')
        if n=='01_base':cq.exporters.export(m.normalized(s),str(stl),tolerance=.06,angularTolerance=.12)
        else:shutil.copyfile(V/row['STL'],stl)
        out.append({**row,'volume_mm3':s.Volume()if n in ['01_base','10_serre_nappe']else row['volume_mm3'],'status':'CANDIDAT_IMPRESSION_PARTIELLE','STEP':'cad/'+n+'.step','STL':'stl/'+n+'.stl','STL_identical_to_V24':sha(stl)==sha(V/row['STL']),'STL_sha256':sha(stl),'STEP_identical_to_V24':dst.read_bytes()==old.read_bytes()})
    assert len(out)==14 and sum(q['quantity_to_print_two_kits']for q in out)==40
    (P/'preuves/pieces.json').write_text(json.dumps(out,indent=2,ensure_ascii=False)+'\n')
    print(json.dumps({'printable_types':14,'parts_two_kits':40,'own_instances':len(items),'changed_individual_STEP':['01_base','10_serre_nappe'],'unchanged_STL_V24':sum(q['STL_identical_to_V24']for q in out)},indent=2),flush=True)
if __name__=='__main__':main()
