"""V26 printable adjustment kit. Own parts only; vendor solids stay local.

CAD X=KiCad X, CAD Y=-KiCad Y, Nucleo top Z0, millimetres.
Released for prototype printing with quantified cold fitting, not physical use.
"""
from pathlib import Path
import importlib.util,json,hashlib,shutil,math
import cadquery as cq
P=Path(__file__).resolve().parents[1];R=P.parent;V25=R/'V2_5_KISS_FIXATIONS';V24=R/'V2_4_IMPRESSION_3D'
spec=importlib.util.spec_from_file_location('v24_geometry',V24/'sources/modele.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
box=m.box;cyl=m.cyl;capsule=m.capsule
SHIELD_TOP=13.85;ROOF=38.;SHOULDER=34.5;GUIDE_BOTTOM=23.5
CONFIG={
 'MKI':{'anchors':[[39,20],[71,54.5]],'guides':[[44,16],[38.5,48.5]],'types':['32_patin_MKI_avant','33_patin_MKI_arriere'],'contacts':[[48,23.4],[48,52.5]],'paths':[[[39,20],[44,16]],[[39,20],[38.5,55.25]],[[38.5,55.25],[71,55.25]],[[71,55.25],[71,54.5]]]},
 'Click':{'anchors':[[91,75],[94.5,112.5]],'guides':[[78.3,86.75],[78.3,96.75]],'types':['34_patin_Click','34_patin_Click'],'contacts':[[84.2,86.75],[84.2,96.75]],'paths':[[[91,75],[78.3,75]],[[78.3,75],[78.3,112.5]],[[78.3,112.5],[94.5,112.5]]]}}
PARTS={}
def part(n,s,q,role,orientation='flat'):
    s=s.clean();assert s.isValid()and len(s.Solids())==1,(n,'not one valid solid')
    PARTS[n]={'shape':s,'qty':q,'role':role,'orientation':orientation};return s
def bearing(h):
    s=cq.Workplane('XY').rect(4.4,4).extrude(h).edges('|Z').fillet(.4).val()
    return s.cut(cyl(0,0,-.1,1.2,h+.2))
def guide(x,y,kind,s=None):
    # 5x5 sliding shoe, 0.2 nominal clearance each face; steel only on PETG.
    if s is None:s=box(x-4.2,y-4.2,GUIDE_BOTTOM,x+4.2,y+4.2,ROOF)
    s=s.cut(box(x-2.7,y-2.7,23.4,x+2.7,y+2.7,32))
    s=s.cut(m.hexhole(x,y,32.4,5.9,4.4)).cut(cyl(x,y,31.9,1.8,7))
    # Ordinary M3 DIN985 nut (H4), accessible before mounting the boards.
    if kind=='front':
        s=s.cut(box(x-3.4,y-5,32.4,x+3.4,y,36.8))
        s=s.cut(box(x-1.2,y+2.6,23.4,x+3.5,y+4.3,32.2))
    elif kind=='rear':
        s=s.cut(box(x-5,y-3,32.4,x,y+3,36.8))
        s=s.cut(box(x-1.2,y+2.6,23.4,x+1.2,y+4.3,32.2))
    else:
        s=s.cut(box(x-5,y-3,32.4,x,y+3,36.8))
        s=s.cut(box(x+2.6,y-1.2,23.4,x+4.3,y+1.2,32.2))
    return s
def frame(which):
    c=CONFIG[which];pieces=[]
    for a,b in c['paths']:
        # Rear MKI rail fits the measured gap to FRAM, rather than crossing it.
        thin=which=='MKI'and a[1]==55.25 and b[1]==55.25
        pieces.append(capsule(a,b,.8 if thin else 2,33,5))
    for x,y in c['anchors']:pieces.append(cyl(x,y,SHIELD_TOP,2.8,SHOULDER-SHIELD_TOP))
    for i,(x,y)in enumerate(c['guides']):pieces.append(guide(x,y,'front'if which=='MKI'and i==0 else'rear'if which=='MKI'else'straight'))
    s=pieces[0].fuse(*pieces[1:]).clean()
    # Reapply cavities after union: rails must not refill nut/slider pockets.
    for i,(x,y)in enumerate(c['guides']):s=guide(x,y,'front'if which=='MKI'and i==0 else'rear'if which=='MKI'else'straight',s)
    for x,y in c['anchors']:
        # Counter-nuts on a shoulder, not above the bridge: ordinary M2x30 fits.
        s=s.cut(cyl(x,y,SHOULDER,2.7,10)).cut(cyl(x,y,13.7,1.2,29))
    return s.clean()
def shoe(which):
    s=box(-2.5,-2.5,0,2.5,2.5,2.5).cut(cyl(0,0,1.5,1.7,1.2))
    if which=='front':
        beam=capsule([0,0],[4,7.4],1,0,2.5)
        beam=beam.cut(box(-2,6.2,-.1,6,10,1))
        toe=box(3,7,0,5,7.8,1.25)
    elif which=='rear':
        beam=box(-1,0,0,1,7.5,2.5).fuse(box(0,5.5,0,10.5,7.5,2.5),box(8.5,4,1,10.5,6.5,2.5))
        toe=box(8.5,3.6,0,10.5,4.4,1.25)
    else:
        beam=box(0,-1,1,6.3,1,2.5)
        toe=box(5.5,-1,0,6.3,1,1.25)
    return s.fuse(beam,toe).cut(cyl(0,0,1.5,1.7,1.2)).clean()
def build():
    for h,n in [(8,'20_appui_H08'),(10,'21_appui_H10'),(12,'22_appui_H12')]:part(n,bearing(h),8,'8 appuis à choisir parmi les trois hauteurs, face supérieure ponçable de0..0.5mm')
    for h,n,q in [(.5,'23_cale_0p5',12),(1,'24_cale_1',36),(2,'25_cale_2',12)]:part(n,bearing(h),q,'Calage rigide; H1 sert aussi de rondelle isolante sous têtes et au-dessus modules')
    for which,n in [('MKI','30_etrier_MKI'),('Click','31_etrier_Click')]:part(n,frame(which),2,'Un étrier par module; contacts PETG réglables, jamais métal sur PCB','upside_down')
    for k,n,q in [('front','32_patin_MKI_avant',4),('rear','33_patin_MKI_arriere',4),('straight','34_patin_Click',6)]:part(n,shoe(k),q,'Patin guidé, plan de contact0.8x2mm, coupelle fermée1.5mm; inclut réserves','upside_down')
def placed_new(H=11,top=None):
    if not PARTS:build()
    pcbtop=SHIELD_TOP+H+1.6 if top is None else top
    rows=[]
    for which,n in [('MKI','30_etrier_MKI'),('Click','31_etrier_Click')]:
        rows.append((n,PARTS[n]['shape']))
        for i,((x,y),pn)in enumerate(zip(CONFIG[which]['guides'],CONFIG[which]['types'])):rows.append((which+'_shoe_'+str(i),PARTS[pn]['shape'].translate((x,-y,pcbtop))))
    return rows
def trim(f):
    b=f.read_bytes();c=b'\n'.join(x.rstrip()for x in b.splitlines())+b'\n';assert b.split()==c.split();f.write_bytes(c)
def main():
    build();rows=[]
    inherited=json.loads((V25/'preuves/pieces.json').read_text())
    for q in inherited:
        n=q['id'];sf=P/'stl'/(n+'.stl');cf=P/'cad'/(n+'.step');shutil.copyfile(V25/q['STL'],sf);shutil.copyfile(V25/q['STEP'],cf)
        rows.append({**q,'STL':'stl/'+sf.name,'STEP':'cad/'+cf.name,'status':'IMPRIMABLE_AJUSTEMENT_A_FROID','inherited_exact_V25':True})
    for n,r in PARTS.items():
        pr=m.normalized(r['shape'],r['orientation']);sf=P/'stl'/(n+'.stl');cf=P/'cad'/(n+'.step')
        cq.exporters.export(pr,str(sf),tolerance=.025,angularTolerance=.12);cq.exporters.export(r['shape'],str(cf));trim(cf);b=pr.BoundingBox()
        rows.append({'id':n,'quantity_to_print_two_kits':r['qty'],'STL':'stl/'+sf.name,'STEP':'cad/'+cf.name,'status':'IMPRIMABLE_AJUSTEMENT_A_FROID','orientation':r['orientation'],'volume_mm3':r['shape'].Volume(),'dimensions_print_mm':[b.xlen,b.ylen,b.zlen],'role':r['role'],'inherited_exact_V25':False})
    asm=cq.Assembly(name='V26_OWN_PARTS_NOMINAL_H11_NOT_PHYSICAL_QUALIFICATION')
    asm.add(cq.importers.importStep(str(V25/'cad/assemblage-imprimable.step')).val(),name='coffret_V25_inchange')
    for n,s in placed_new():asm.add(s,name=n)
    for j,(x,y)in enumerate(m.C['module_supports_FRAM']+m.C['module_supports_SD']):
        asm.add(PARTS['21_appui_H10']['shape'].translate((x,-y,13.85)),name='appui_'+str(j))
        asm.add(PARTS['24_cale_1']['shape'].translate((x,-y,23.85)),name='cale_H1_'+str(j))
        asm.add(PARTS['24_cale_1']['shape'].translate((x,-y,26.45 if j<2 else 26.42)),name='rondelle_module_'+str(j))
    for j,(x,y)in enumerate(m.C['module_supports_FRAM']+m.C['module_supports_SD']+CONFIG['MKI']['anchors']+CONFIG['Click']['anchors']):asm.add(PARTS['24_cale_1']['shape'].translate((x,-y,11.25)),name='rondelle_sous_shield_'+str(j))
    asm.save(str(P/'cad/assemblage-pieces-propres.step'));trim(P/'cad/assemblage-pieces-propres.step')
    for q in rows:q['STL_sha256']=hashlib.sha256((P/q['STL']).read_bytes()).hexdigest()
    (P/'preuves/pieces.json').write_text(json.dumps(rows,indent=2,ensure_ascii=False)+'\n')
    params={'mission_id':'TR2-20261105-SHIELD-V26-IMPRESSION-COMPLETE-SANS-RECEPTION','G':12.25,'shield_top':SHIELD_TOP,'module_height_hypothesis_mm':11,'FRAM_SD_mechanical_H_range_mm':[8,15.5],'shim_series_mm':[.5,1,2],'support_heights_mm':[8,10,12],'sanding_max_mm':.5,'PCB_top_for_retainer_range_mm':[24.75,29.15],'plunger_body_height':2.5,'PETG_floor_under_M3_tip':1.5,'M3_adjusting_screw_length':12,'adjusting_nut':'DIN985 M3 AF5.5 H4, nylon ring upward','frame_roof_Z':ROOF,'anchor_nut_shoulder_Z':SHOULDER,'guides':CONFIG,'physical_qualification':False,'release_criterion':'Complete prototype printing kit, mandatory adjustable dry fit before power'}
    (P/'sources/parametres.json').write_text(json.dumps(params,indent=2,ensure_ascii=False)+'\n')
    print(json.dumps({'types':len(rows),'pieces':sum(q['quantity_to_print_two_kits']for q in rows),'new_types':len(PARTS),'inherited_exact_types':len(inherited)},indent=2),flush=True)
if __name__=='__main__':main()
