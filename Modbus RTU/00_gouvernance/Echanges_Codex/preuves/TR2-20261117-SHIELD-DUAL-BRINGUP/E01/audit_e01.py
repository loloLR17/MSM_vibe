"""Compare a previously frozen independent reference to schematic, PCB and CAM.

Gerber geometry support is deliberately limited to the actual supplied files:
metric4.6, positive polarity, circular apertures, straight strokes, filled regions.
Any other drawing instruction fails rather than being silently ignored.
"""
from pathlib import Path
from collections import Counter, defaultdict
import csv
import hashlib
import io
import json
import re
import zipfile
import xml.etree.ElementTree as ET
from shapely.geometry import Point, LineString, Polygon
from shapely.ops import unary_union
from shapely import make_valid
from shapely.strtree import STRtree

HERE=Path(__file__).resolve().parent
ROOT=next(p for p in HERE.parents if (p/'AGENTS.md').is_file())
BOARD=ROOT/'Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_3_ARNACO'
CAM=BOARD/'exports/CAM_CANDIDAT_NON_LIBERE'
reference=json.loads((HERE/'reference-constructeurs.json').read_text())
native=json.loads((HERE/'native-PCB.json').read_text())
pcb={(p['ref'],p['pin']):p for p in native['pads'] if p['pin']}
assert len(pcb)==180
assert hashlib.sha256((BOARD/'cad/TR2_Shield_Proto_V1.kicad_pcb').read_bytes()).hexdigest()==native['source_sha256']
schroot=ET.parse(HERE/'native-schematic-netlist.xml').getroot()
sch={(n.attrib['ref'],n.attrib['pin']):dict(net=net.attrib['name'], **n.attrib)
     for net in schroot.findall('./nets/net') for n in net.findall('node')}
assert set(sch)==set(pcb)=={(r['ref'],r['pin']) for r in reference['rows']}

def normal(net):
    return 'NC' if net in ['N/C',''] or net.startswith('unconnected-') else net

def parse_gerber(path):
    apertures={}; attrs={}; flashes={}; objects=[]; at=(0.,0.); aperture=None; region=None; contours=[]
    text=path.read_text()
    assert '%FSLAX46Y46*%' in text and '%MOMM*%' in text and '%LPD*%' in text
    assert '%LPC' not in text and '%AM' not in text and not re.search(r'^G0[23]',text,re.M)
    for line in text.splitlines():
        if line.startswith('%ADD'):
            m=re.fullmatch(r'%ADD(\d+)C,([\d.]+)\*%',line);assert m,line
            apertures[int(m[1])]=float(m[2]);continue
        if line=='%TD*%':attrs={};continue
        if line.startswith('%TO.P,'):attrs['pad']=tuple(line[6:-2].split(',')[:2]);continue
        if line.startswith('%TO.N,'):attrs['net']=line[6:-2];continue
        if line=='G36*':region=[];contours=[];continue
        if line=='G37*':
            if region:contours.append(region)
            shape=None
            for contour in contours:
                part=make_valid(Polygon(contour))
                shape=part if shape is None else shape.symmetric_difference(part)
            assert shape is not None
            objects.append(shape);region=None;continue
        m=re.fullmatch(r'D(\d+)\*',line)
        if m:aperture=int(m[1]);assert aperture in apertures;continue
        m=re.fullmatch(r'X(-?\d+)Y(-?\d+)D0([123])\*',line)
        if m:
            end=(int(m[1])/1e6,-int(m[2])/1e6);op=m[3]
            if region is not None:
                if op=='2' and region:contours.append(region);region=[]
                assert op in ['1','2'];region.append(end)
            elif op=='1':objects.append(LineString([at,end]).buffer(apertures[aperture]/2,quad_segs=32))
            elif op=='3':
                objects.append(Point(end).buffer(apertures[aperture]/2,quad_segs=32))
                if 'pad' in attrs:flashes[attrs['pad']]=dict(net=attrs.get('net','N/C'),xy=end,diameter=apertures[aperture])
            at=end;continue
        if not line or line.startswith(('G04','%TF.','%TA.','%TD','%FS','%MO','%LP','%TO.')) or line in ['G01*','M02*']:continue
        raise AssertionError('Unsupported Gerber instruction '+line)
    return flashes,unary_union(objects)

cam={};geometry={};camresults={}
for layer,suffix in [('F','F_Cu.gtl'),('B','B_Cu.gbl')]:
    flashes,geo=parse_gerber(CAM/('TR2_Shield_Proto_V1-'+suffix))
    assert set(flashes)==set(pcb)
    for key,item in pcb.items():
        flash=flashes[key]
        assert abs(flash['xy'][0]-item['x'])<0.00001 and abs(flash['xy'][1]-item['y'])<0.00001
        assert normal(flash['net'])==normal(item['net'])
        assert abs(flash['diameter']-item['size_x'])<0.00001
    cam[layer]=flashes;geometry[layer]=geo
    camresults[layer]={'pad_attributes_and_positions_match':180,'polygon_parts':len(geo.geoms) if hasattr(geo,'geoms') else 1}

# Independent physical connectivity from final copper, not X2 labels alone.
parts={layer:list(g.geoms) if hasattr(g,'geoms') else [g] for layer,g in geometry.items()}
trees={layer:STRtree(g) for layer,g in parts.items()}
parent={}
def find(x):
    parent.setdefault(x,x)
    if parent[x]!=x:parent[x]=find(parent[x])
    return parent[x]
def join(a,b):parent[find(a)]=find(b)
def component(layer,xy):
    pt=Point(xy);ids=[int(i) for i in trees[layer].query(pt) if parts[layer][int(i)].covers(pt)]
    assert len(ids)==1,(layer,xy,ids)
    return (layer,ids[0])
for q in pcb.values():join(component('F',(q['x'],q['y'])),component('B',(q['x'],q['y'])))
for t in native['tracks']:
    if t['via']:join(component('F',t['start']),component('B',t['start']))
groups=defaultdict(list)
for key,pad in pcb.items():groups[find(component('F',(pad['x'],pad['y'])))].append(key)
shorts=[];opens=[]
for keys in groups.values():
    nets={pcb[key]['net'] for key in keys}
    if len(nets)>1:shorts.append({'pads':keys,'nets':sorted(nets)})
by_net=defaultdict(set)
for key,pad in pcb.items():by_net[pad['net']].add(find(component('F',(pad['x'],pad['y']))))
opens=[net for net,components in by_net.items() if len(components)>1]
assert not shorts and not opens,(shorts,opens)

# Delivered candidate archive: all nine exact bytes; actual factory order hash is unknown.
manifest=json.loads((BOARD/'preuves/CAM-manifeste.json').read_text())
archive=BOARD/'exports'/manifest['zip']
assert hashlib.sha256(archive.read_bytes()).hexdigest()==manifest['zip_SHA256']
with zipfile.ZipFile(archive) as z:
    assert sorted(z.namelist())==sorted(manifest['files'])
    for name,sha in manifest['files'].items():
        data=z.read(name);assert hashlib.sha256(data).hexdigest()==sha
        assert data==(CAM/name).read_bytes()

# Drill centers from delivered Excellon vs all native plated/non-plated holes.
for kind,npth in [('PTH',False),('NPTH',True)]:
    want=Counter((round(p['x'],3),round(p['y'],3),round(p['drill'],3)) for p in native['pads'] if p['npth']==npth)
    if not npth:
        for t in native['tracks']:
            if t['via']:want[(round(t['start'][0],3),round(t['start'][1],3),0.3)]+=1
    tools={};got=Counter();tool=None
    for line in (CAM/f'TR2_Shield_Proto_V1-{kind}.drl').read_text().splitlines():
        m=re.fullmatch(r'T(\d+)C([\d.]+)',line)
        if m:tools[m[1]]=float(m[2]);continue
        if re.fullmatch(r'T\d+',line):tool=line[1:];continue
        m=re.fullmatch(r'X(-?[\d.]+)Y(-?[\d.]+)',line)
        if m:got[(round(float(m[1]),3),round(-float(m[2]),3),tools[tool])]+=1
    assert got==want,(kind,got-want,want-got)

rows=[]
for original in reference['rows']:
    r=dict(original);key=r['ref'],r['pin'];want=r['desired_net']
    r.update(schematic_pin_function=sch[key].get('pinfunction',''),schematic_pin_type=sch[key].get('pintype',''),
             schematic_net=normal(sch[key]['net']),pcb_net=normal(pcb[key]['net']),
             CAM_F=normal(cam['F'][key]['net']),CAM_B=normal(cam['B'][key]['net']),
             pcb_x_mm=pcb[key]['x'],pcb_y_mm=pcb[key]['y'],
             copper_component_verified=True,physical_status='INCONNU — continuity not measured for this pad')
    r['verdict']='PASS' if all(r[x]==want for x in ['schematic_net','pcb_net','CAM_F','CAM_B']) else 'FAIL'
    r['consequence']='Matches independent intended connection (documentary only)' if r['verdict']=='PASS' else 'Wrong connection or missing required connection; blocks power/release'
    rows.append(r)
out={'scope':'180/180 functional/NC connector and passive/TP pads; source reference frozen separately',
     'PCB_sha256':native['source_sha256'],'reference_sha256':hashlib.sha256((HERE/'reference-constructeurs.json').read_bytes()).hexdigest(),
     'coverage':dict(Counter(x['verdict'] for x in rows)),
     'CAM':{'layers':camresults,'geometric_groups':len(groups),'shorts':shorts,'opens_vs_native_netlist':opens,
            'method':'All dark circular flashes/linear strokes/filled regions unioned; connected components bridged only by every PTH and4vias; names checked after geometry',
            'resolution':'metric1um coordinates; circles approximated128segments; not fab metrology',
            'archive_sha256':manifest['zip_SHA256'],'archive_exact_members':9,'actual_uploaded_order_identity':'INCONNU — no order receipt/upload hash in repository'},
     'rows':rows,'release':'NO-GO; manufacturer review and actual hardware gates outstanding'}
(HERE/'audit-complet.json').write_text(json.dumps(out,indent=2,ensure_ascii=False)+'\n')
with (HERE/'MATRICE_E01_180_CONTACTS.csv').open('w',newline='') as f:
    writer=csv.DictWriter(f,fieldnames=list(rows[0]));writer.writeheader();writer.writerows(rows)
print(json.dumps({k:v for k,v in out.items() if k!='rows'},indent=2,ensure_ascii=False))
