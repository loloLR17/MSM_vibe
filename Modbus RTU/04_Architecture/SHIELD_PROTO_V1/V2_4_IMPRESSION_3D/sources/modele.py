"""V24 own mechanical geometry. No ST/manufacturer solids are exported here.

CAD X=KiCad X, CAD Y=-KiCad Y, Nucleo PCB top Z=0, units mm.
Unknown module bearing zones are explicitly excluded from the printer pack.
"""
from pathlib import Path
import json, math, hashlib
import cadquery as cq
from shapely.geometry import LineString
from shapely.ops import unary_union

P=Path(__file__).resolve().parents[1]
C=json.loads((P/'sources/parametres.json').read_text())
D=json.loads((P/'sources/geometrie-reference.json').read_text())
PARTS={}; INST=[]

def box(x,y,z,X,Y,Z):
    assert X>x and Y>y and Z>z
    return cq.Solid.makeBox(X-x,Y-y,Z-z,cq.Vector(x,-Y,z))

def cyl(x,y,z,r,h):return cq.Solid.makeCylinder(r,h,cq.Vector(x,-y,z))
def hole(s,x,y,z,r,h):return s.cut(cyl(x,y,z,r,h))
def capsule(a,b,r,z,h):
    dx,dy=b[0]-a[0],b[1]-a[1];L=math.hypot(dx,dy);nx=-dy/L*r;ny=dx/L*r
    pts=[(a[0]+nx,-a[1]-ny),(b[0]+nx,-b[1]-ny),(b[0]-nx,-b[1]+ny),(a[0]-nx,-a[1]-ny)]
    # Correct polygon's last point is on the other side of a.
    pts[-1]=(a[0]-nx,-a[1]+ny)
    return cq.Workplane('XY').polyline(pts).close().extrude(h).val().translate((0,0,z)).fuse(cyl(*a,z,r,h),cyl(*b,z,r,h)).clean()

def hexhole(x,y,z,af,h):
    return cq.Workplane('XY').polygon(6,af/(math.sqrt(3)/2)).extrude(h).val().translate((x,-y,z))

def captive(s,x,y,top,bottom):
    s=hole(s,x,y,bottom,1.8,top-bottom+.2)
    # Nut pocket with side access, not a printed thread.
    z=top-5.2
    s=s.cut(hexhole(x,y,z,5.9,2.8))
    s=s.cut(box(x-6,y-3.0,z,x,y+3.0,z+2.8))
    return s

def normalized(s,orientation='flat'):
    if orientation=='north':s=s.rotate((0,0,0),(1,0,0),-90)
    if orientation=='south':s=s.rotate((0,0,0),(1,0,0),90)
    if orientation=='west':s=s.rotate((0,0,0),(0,1,0),-90)
    if orientation=='east':s=s.rotate((0,0,0),(0,1,0),90)
    if orientation=='upside_down':s=s.rotate((0,0,0),(1,0,0),180)
    bb=s.BoundingBox()
    return s.translate((-bb.xmin,-bb.ymin,-bb.zmin))

def part(n,s,qty,used,status='PRET_A_IMPRIMER',orientation='flat',note=''):
    s=s.clean();assert s.isValid() and len(s.Solids())==1,(n,'invalid or disconnected')
    pr=normalized(s,orientation);assert pr.isValid()
    PARTS[n]={'shape':s,'print':pr,'qty':qty,'used_two_kits':used,'status':status,'orientation':orientation,'note':note}
    return s

def instance(n,s,label=None):INST.append({'part':n,'label':label or n,'shape':s})

def build():
    # Shell: same 158x166 plan as V15. Deeper floor avoids official C05 underside.
    base=box(8,-20,-14,166,146,47).cut(box(11,-17,-11,163,143,48))
    for x,y in C['lid_screw_centres']:
        base=base.fuse(cyl(x,y,-11,5,58));base=captive(base,x,y,47,-13.7)
    # Three intended PCB bearing cups; generous unverified Ajile head allowance.
    for j,(x,y) in enumerate(C['anchors']):
        post=cyl(x,y,-11,5,9.4).cut(cyl(x,y,-4.8,3.2,3.3))
        # Half-cups follow the D126 opening direction: free the actual nearby
        # underside pins, not just the PCB outline. No full ring substitution.
        post=post.cut(box(x-6,y if j==0 else y-6,-11.1,x+6,y+6 if j==0 else y,-1.5))
        base=base.fuse(post)
    for x,y in C['case_keeper_pivots']:
        post=captive(cyl(x,y,-11,4.5,12.4),x,y,1.4,-13.7)
        base=base.fuse(post)
    # Case-mounted RS485 cable clamp, wholly beyond Click board's nominal end.
    for x,y in [(154,82),(154,101)]:
        post=captive(cyl(x,y,-11,5,36),x,y,25,10)
        base=base.fuse(post)

    # Open-to-top bays avoid long PETG bridges. Sliding panels retained by lid.
    bays=[('nord',27,91,-7),('sud',20,90,-7),('SD',78,114,19),('RS485',75,110,21)]
    panels={}
    for wall,u,U,z in bays:
        if wall=='nord':
            base=base.cut(box(u,-21,z,U,-12,48))
            for a,b in [(u-4,u),(U,U+4)]:base=base.fuse(box(a,-20,z-2,b,-12,47))
            for a,b in [(u-2,u+.1),(U-.1,U+2)]:base=base.cut(box(a,-18.4,z,b,-15.6,47.2))
            panel=box(u-1.8,-18.2,z+.2,U+1.8,-15.8,46.75)
        elif wall=='sud':
            base=base.cut(box(u,138,z,U,147,48))
            for a,b in [(u-4,u),(U,U+4)]:base=base.fuse(box(a,138,z-2,b,146,47))
            for a,b in [(u-2,u+.1),(U-.1,U+2)]:base=base.cut(box(a,141.6,z,b,144.4,47.2))
            panel=box(u-1.8,141.8,z+.2,U+1.8,144.2,46.75)
        elif wall=='SD':
            base=base.cut(box(7,u,z,16,U,48))
            for a,b in [(u-4,u),(U,U+4)]:base=base.fuse(box(8,a,z-2,16,b,47))
            for a,b in [(u-2,u+.1),(U-.1,U+2)]:base=base.cut(box(9.6,a,z,12.4,b,47.2))
            panel=box(9.8,u-1.8,z+.2,12.2,U+1.8,46.75)
        else:
            base=base.cut(box(158,u,z,167,U,48))
            for a,b in [(u-4,u),(U,U+4)]:base=base.fuse(box(158,a,z-2,166,b,47))
            for a,b in [(u-2,u+.1),(U-.1,U+2)]:base=base.cut(box(161.6,a,z,164.4,b,47.2))
            panel=box(161.8,u-1.8,z+.2,164.2,U+1.8,46.75)
        panels[wall]=panel

    # North service cassette: broad USB, separate clear LED window and ribbon.
    north=panels['nord'].cut(box(55,-19,-5,84,-15,10))
    north=north.cut(box(35,-19,13,80,-15,27))
    north=north.cut(box(46,-19,32,61,-15,46))
    for x in [30.5,85]:
        cutter=cq.Solid.makeCylinder(1.8,5,cq.Vector(x,19,20),cq.Vector(0,-1,0));north=north.cut(cutter)
    for x in [42.5,64.5]:
        # Slotted clamp position in Z permits a simple cable route adjustment.
        for z in [36,40]:north=north.cut(cq.Solid.makeCylinder(1.8,5,cq.Vector(x,19,z),cq.Vector(0,-1,0)))
        north=north.cut(box(x-1.8,-19,36,x+1.8,-15,40))
    # Window ring traps an unperforated rectangular 50x18x1.5 clear PC sheet.
    frame=box(27.4,-14.3,10,88.5,-12.3,30).cut(box(35,-15,13,80,-12,27))
    for x in [30.5,85]:frame=frame.cut(cq.Solid.makeCylinder(1.8,4,cq.Vector(x,15,20),cq.Vector(0,-1,0)))
    # Ribbon strain-relief saddle: nylon tie through two slots holds the ribbon
    # against a soft-lined bar. The cassette aperture is not a clamping anvil.
    ribbon=box(39.5,-11.0,35,67.5,-8.0,41)
    for x in [42.5,64.5]:ribbon=ribbon.cut(cq.Solid.makeCylinder(1.8,5,cq.Vector(x,12,38),cq.Vector(0,-1,0)))
    for x in [47,60]:ribbon=ribbon.cut(box(x-.9,-12,36.4,x+.9,-7,39.6))
    # South direct USER access, no actuation rod; USB-C separate and RESET offset.
    south=panels['sud'].cut(box(53,141,-5,81,145,10)).cut(box(22,141,2,43,145,24))
    south=south.cut(cq.Solid.makeCylinder(1.8,5,cq.Vector(45,-146,27),cq.Vector(0,1,0)))
    user=box(20,144.4,0,49,146.8,31).cut(cq.Solid.makeCylinder(1.8,4,cq.Vector(45,-148,27),cq.Vector(0,1,0)))
    sd=panels['SD'].cut(box(9,82,21,13,110,39))
    sd=sd.cut(cq.Solid.makeCylinder(1.8,5,cq.Vector(8,-96,43),cq.Vector(1,0,0)))
    sd_door=box(7.2,80,19,9.6,112,46.3).cut(cq.Solid.makeCylinder(1.8,4,cq.Vector(6,-96,43),cq.Vector(1,0,0)))
    rs=panels['RS485'].cut(box(161,80,25,165,104,42.5))
    rsbar=capsule([154,82],[154,101],3,27,3)
    for x,y in [(154,82),(154,101)]:rsbar=hole(rsbar,x,y,26,1.8,5)
    part('01_base',base,2,2,note='Fond -14, plancher -11; baies en U ouvertes en haut; poches écrous M3, pas de filets imprimés')
    instance('01_base',base)
    for n,s,ori in [('03_cassette_nord',north,'north'),('04_cassette_sud',south,'south'),('05_cassette_SD',sd,'west'),('06_cassette_RS485',rs,'east'),('07_cadre_vitre',frame,'north'),('08_trappe_USER',user,'south'),('09_trappe_SD',sd_door,'west'),('10_serre_nappe',ribbon,'north'),('11_serre_cable_RS485',rsbar,'flat')]:
        part(n,s,2,2,orientation=ori);instance(n,s)

    lid=box(8,-20,47,166,146,50)
    for x,y in C['lid_screw_centres']:lid=hole(lid,x,y,46,1.8,5)
    # Flush tool hatch. No downward bosses: inner lid face prints flat.
    lid=lid.cut(box(131,78,46,145,107,51)).cut(box(124,74,48,153,111,51))
    hatch=box(124.3,74.3,48,152.7,110.7,50)
    for x in [127.5,149.5]:lid=hole(lid,x,92.5,46,1.8,5);hatch=hole(hatch,x,92.5,47,1.8,4)
    # Hollow hatch opening itself must remain open below its removable cap.
    # ARNACO: exact V23 strokes, six polygons fused into this actual lid solid.
    word=json.loads((P/'sources/ARNACO-traits.json').read_text())
    ink=unary_union([LineString(pair).buffer(.125,resolution=8)for pair in word['segments']])
    polys=list(ink.geoms) if ink.geom_type=='MultiPolygon' else [ink]
    assert len(polys)==6
    scale=word['scale'];cx=(ink.bounds[0]+ink.bounds[2])/2;cy=(ink.bounds[1]+ink.bounds[3])/2
    area=0;glyphs=[]
    for poly in polys:
        def pts(ring):return [(87+(x-cx)*scale,-(59+(y-cy)*scale))for x,y in list(ring.coords)[:-1]]
        body=cq.Workplane('XY').polyline(pts(poly.exterior)).close().extrude(1.1).val().translate((0,0,49.9))
        for ring in poly.interiors:
            body=body.cut(cq.Workplane('XY').polyline(pts(ring)).close().extrude(1.3).val().translate((0,0,49.8)))
        glyphs.append(body);area+=poly.area*scale*scale
    plain_volume=lid.Volume();lid=lid.fuse(*glyphs).clean()
    part('02_couvercle_ARNACO',lid,2,2,note='Relief ARNACO intégré1mm, six glyphes, aucune devise; face intérieure à plat')
    instance('02_couvercle_ARNACO',lid)
    part('12_trappe_outil_RS485',hatch,2,2);instance('12_trappe_outil_RS485',hatch)

    # D126 unchanged analytic profile, 6 useful plus2spares, flat on end face.
    d126=cyl(0,0,0,3,12.25).cut(cyl(0,0,-.1,1.2,12.45)).cut(box(-5,-5,-1,5,0,14))
    # In CAD this removes Y>=0, identical to V21's half-annulus opening.
    part('13_D126',d126,8,6,note='12,25±0,10 à accepter après impression; couches0,10 et100%; ouvrir vers CN15 en haut, STLINK en bas')
    for j,(x,y)in enumerate(C['anchors']):instance('13_D126',d126.rotate((0,0,0),(0,0,1),180 if j==0 else 0).translate((x,-y,0)),f'D126_{j}')
    # Height-independent insulating bearings; lower jam nuts set actual module fit.
    washer=cyl(0,0,0,2.5,1.4).cut(cyl(0,0,-.1,1.2,1.6))
    part('14_appui_module_reglable_M2',washer,12,8,note='8appuis+4réserves; M2x20 avec2écrous inférieurs contrebloqués et1supérieur; aucune hauteur11mm imposée')
    for x,y in C['module_supports_FRAM']+C['module_supports_SD']:instance('14_appui_module_reglable_M2',washer.translate((x,-y,23.45)),f'appui_{x}_{y}')
    # Universal slotted anti-lift arm. Shoe gap0.8mm, no PCB preload.
    keeper=capsule([7.6,0],[14,0],3.4,0,3).fuse(capsule([14,0],[20,0],1.5,0,3),cyl(20,0,-.6,1.2,.7))
    keeper=keeper.cut(capsule([7,0],[10.5,0],1.8,-1,5))
    part('15_retenue_Nucleo',keeper,8,6,orientation='upside_down',note='6utiles+2réserves, lumièreM3; jeu antilevée0,8 nominal sans serrer le PCB')
    for j,((px,py),(ax,ay))in enumerate(zip(C['case_keeper_pivots'],C['anchors'])):
        right=j==2;contact=ax+5.6 if right else ax-5.3;distance=abs(px-contact);local_pivot=20-distance
        assert 7<local_pivot<10.5
        k=keeper.rotate((0,0,0),(0,0,1),180 if right else 0).translate((px+local_pivot if right else px-local_pivot,-py,1.4))
        instance('15_retenue_Nucleo',k,f'retenue_{j}')

    # Complete bridge bodies with height adjustment by nylon screws, but contact
    # zones on real MKI/Click remain unproved -> not in printer archive.
    for name,anchors in [('16_etrier_MKI',C['MKI_retainer_anchors']),('17_etrier_Click',C['Click_retainer_anchors'])]:
        z=13.85;s=capsule(*anchors,3,z+22,4)
        for x,y in anchors:s=s.fuse(cyl(x,y,z,3,26))
        for j,(x,y) in enumerate(anchors):s=hole(s,x,y,z-.1,1.4 if j==0 else 1.8,26.2)
        if 'MKI' in name:contacts=[[43.1,24.3],[62.1,44.9]]
        else:
            s=s.fuse(capsule([84.1,91.43],[91,91.43],3,z+22,4));contacts=[[84.1,91.43]]
        for x,y in contacts:s=hole(s,x,y,z+21,1.4,6)
        part(name,s,2,2,'A_VALIDER_SUR_MATERIEL','upside_down','Corps vérifiable; zones nues/contact et composants réels non démontrés. Vis de contact réglables, aucun patin fixe inventé.')
        instance(name,s)

    # Reference envelopes are not printable and never included in operator pack.
    ref=[]
    shield=box(20,17,12.25,110,117,13.85).cut(cq.Compound.makeCompound([cyl(q['x'],q['y'],12.2,q['drill']/2,1.8)for q in D['pads']]))
    ref.append(('shield',shield))
    poly=[(x,-y)for x,y in D['Nucleo_outline']];ref.append(('Nucleo_outline_only',cq.Workplane('XY').polyline(poly).close().extrude(1.6).val().translate((0,0,-1.6))))
    for name,b in [('FRAM',[42.46,56.46,67.86,74.24]),('SD',[20.18,83.14,43.04,108.54]),('MKI',[42.46,21.46,62.78,54.48]),('Click',[83.41,78.73,140.56,104.13])]:ref.append((name+'_nominal_PCB',box(b[0],b[1],24.85,b[2],b[3],26.45)))
    # Intact ten-way female housings: extra position stays, no tenth PCB tail.
    ref.extend([('socket_FRAM_1x10',box(43.08,57.75,13.85,69.78,60.25,22.35)),('socket_SD_1x10',box(39.25,81.22,13.85,41.75,107.92,22.35))])
    for x in [45,60.24]:ref.append((f'socket_MKI_{x}',box(x-1.25,22.08,13.85,x+1.25,53.86,22.35)))
    for y in [80,102.86]:ref.append((f'socket_Click_{y}',box(85.3,y-1.25,13.85,106.92,y+1.25,22.35)))
    ref.extend([('MKI_IDC_PROVISIONAL',box(43.5,24,26.45,61.5,50,34.85)),('Click_components_PROVISIONAL',box(88,82,26.45,130,101,29.25))])
    for yy in [86.731,97.399]:ref.append((f'Click_terminal_PROVISIONAL_{yy}',box(134.45,yy-2.77,26.45,140.50,yy+2.77,34.94)))
    # Clear pane is a purchased/cut sheet, not printed. No holes or complex notch.
    ref.append(('PC_window_not_printed',box(32.5,-15.8,11,82.5,-14.3,29)))
    return ref,{'glyph_count':len(glyphs),'emboss_area_mm2':area,'plain_lid_volume':plain_volume,'lid_volume':lid.Volume(),'expected_added_relief_volume':area,'relief_height':1,'no_copyright_or_motto':True}

def export():
    ref,logo=build();rows=[]
    asm=cq.Assembly(name='TR2_V24_OWN_PARTS_NOMINAL')
    for row in INST:
        asm.add(row['shape'],name=row['label'])
    asm.save(str(P/'cad/assemblage-pieces-propres.step'))
    for n,r in PARTS.items():
        f=P/'stl'/r['status']/(n+'.stl');cq.exporters.export(r['print'],str(f),tolerance=.025 if n=='13_D126' else .06,angularTolerance=.12)
        cq.exporters.export(r['shape'],str(P/'cad'/(n+'.step')))
        b=r['print'].BoundingBox()
        rows.append({'id':n,'status':r['status'],'quantity_to_print_two_kits':r['qty'],'useful_two_kits':r['used_two_kits'],'spares':r['qty']-r['used_two_kits'],'STL':str(f.relative_to(P)),'STEP':'cad/'+n+'.step','dimensions_print_mm':[b.xlen,b.ylen,b.zlen],'volume_mm3':r['shape'].Volume(),'orientation':r['orientation'],'note':r['note']})
    for f in (P/'cad').glob('*.step'):
        raw=f.read_bytes();clean=b'\n'.join(l.rstrip()for l in raw.splitlines())+b'\n';assert raw.split()==clean.split();f.write_bytes(clean)
    (P/'preuves/pieces.json').write_text(json.dumps(rows,indent=2,ensure_ascii=False)+'\n')
    (P/'preuves/relief-ARNACO.json').write_text(json.dumps(logo,indent=2)+'\n')
    print(json.dumps({'parts':len(rows),'printable_types':sum(r['status']=='PRET_A_IMPRIMER'for r in rows),'excluded_types':sum(r['status']!='PRET_A_IMPRIMER'for r in rows),'logo':logo},indent=2),flush=True)

if __name__=='__main__':export()
