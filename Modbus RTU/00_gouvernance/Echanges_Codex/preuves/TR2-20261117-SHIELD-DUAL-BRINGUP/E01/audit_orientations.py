"""Source geometry versus preserved PCB, including bottom sockets and mirrors.

Read ST native Altium data using the APAD6 layout already qualified in V1.9.
Adafruit coordinates come from native Eagle pads and mounting holes.
Click v101 is explicitly not asserted to be the actual v102 fabrication file.
"""
from pathlib import Path
import hashlib,json,math,struct,xml.etree.ElementTree as ET
import olefile
import sexpdata

HERE=Path(__file__).resolve().parent
ROOT=next(p for p in HERE.parents if (p/'AGENTS.md').is_file())
REPORTS=Path('/home/lolo/dev/msm/reports')
CAD=ROOT/'Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_3_ARNACO/cad'
native=json.loads((HERE/'native-PCB.json').read_text())
padmap={(p['ref'],p['pin']):p for p in native['pads'] if p['pin']}

def records(data):
    i=0;result=[]
    while i<len(data):
        n=struct.unpack_from('<I',data,i)[0];i+=4
        text=data[i:i+n].decode('cp1252').strip('\0');i+=n
        result.append(dict(v.split('=',1) for v in text.split('|') if '=' in v))
    assert i==len(data)
    return result

def altium(path):
    with olefile.OleFileIO(path) as f:
        comps=records(f.openstream('Components6/Data').read())
        nets=records(f.openstream('Nets6/Data').read())
        raw=f.openstream('Pads6/Data').read()
    i=0;out=[]
    while i<len(raw):
        assert raw[i]==2;i+=1;subs=[]
        for _ in range(6):
            size=struct.unpack_from('<I',raw,i)[0];i+=4;subs.append(raw[i:i+size]);i+=size
        name=subs[0][1:1+subs[0][0]].decode('cp1252');d=subs[4];assert len(d)>=110
        comp=struct.unpack_from('<H',d,7)[0];net=struct.unpack_from('<H',d,3)[0]
        x,y=struct.unpack_from('<ii',d,13)
        out.append(dict(ref=comps[comp].get('SOURCEDESIGNATOR','') if comp!=65535 else '__board__',
                        pin=name,x=x*.00000254,y=y*.00000254,
                        net=nets[net].get('NAME','') if net!=65535 else 'NC'))
    assert i==len(raw)
    return out

st=REPORTS/'TR2-shield-v19-20261105/ST-design/MB1549C/MB1549.PcbDoc'
stpads={(x['ref'],x['pin']):x for x in altium(st)}
# Top-view coordinate convention: ST y-up -> KiCad y-down, no physical mirror.
anchor=stpads['CN8','1'];anchor_dst=padmap['J2','1']
tx,ty=anchor_dst['x']-anchor['x'],anchor_dst['y']+anchor['y']
comparison=[]
for ref,cn,count in [('J1','CN7',20),('J2','CN8',16),('J3','CN9',30),('J4','CN10',34),('J5','CN12',6)]:
    for pin in range(1,count+1):
        sp=stpads[cn,str(pin+64 if ref=='J5' else pin)];dp=padmap[ref,str(pin)]
        delta=math.hypot(sp['x']+tx-dp['x'],ty-sp['y']-dp['y'])
        assert delta<.000002,(ref,pin,delta)
        comparison.append(dict(ref=ref,pin=pin,source_pin=f'{cn}-{sp["pin"]}',ST_native_net=sp['net'],delta_mm=delta))

# Compare the actual library footprints to all placed pads, including bottom flip.
board=sexpdata.loads((CAD/'TR2_Shield_Proto_V1.kicad_pcb').read_text())
def children(node,name):return [x for x in node if isinstance(x,list) and x and str(x[0])==name]
lib_checks=[]
for fp in children(board,'footprint'):
    name=str(fp[1]).split(':')[-1]
    props=children(fp,'property');ref=next(str(x[2]) for x in props if str(x[1])=='Reference')
    lib=sexpdata.loads((CAD/'TR2.pretty'/(name+'.kicad_mod')).read_text())
    actual={str(x[1]):x for x in children(fp,'pad') if str(x[1])}
    expected={str(x[1]):x for x in children(lib,'pad') if str(x[1])}
    assert set(actual)==set(expected)
    bottom=str(children(fp,'layer')[0][1])=='B.Cu'
    for num,ap in actual.items():
        aa=children(ap,'at')[0];ea=children(expected[num],'at')[0]
        # KiCad's bottom flip changes library local Y in this preserved design.
        assert abs(float(aa[1])-float(ea[1]))<.000002
        assert abs(float(aa[2])-(-float(ea[2]) if bottom else float(ea[2])))<.000002,(ref,num,aa,ea)
    lib_checks.append(dict(ref=ref,library=name,pads=len(actual),bottom=bottom,status='PASS'))

ada=[]
for name,ref in [('FRAM','J6'),('SD','J7')]:
    path=REPORTS/f'TR2-shield-v26-20261105/{name}-official.brd'
    root=ET.parse(path).getroot();e=root.find(".//board/elements/element[@name='JP1']")
    pkg=root.find(f".//board/libraries/library[@name='{e.attrib['library']}']/packages/package[@name='{e.attrib['package']}']")
    angle=math.radians(float(e.attrib.get('rot','R0').replace('R','')))
    source=[]
    for p in pkg.findall('pad'):
        x,y=float(p.attrib['x']),float(p.attrib['y'])
        source.append(dict(pin=p.attrib['name'],x=x*math.cos(angle)-y*math.sin(angle)+float(e.attrib['x']),
                           y=x*math.sin(angle)+y*math.cos(angle)+float(e.attrib['y'])))
    holes=[(float(v.attrib['x']),float(v.attrib['y'])) for v in root.findall('.//board/elements/element') if 'MOUNTINGHOLE' in v.attrib['package']]
    p1=next(p for p in source if p['pin']=='1');dest=padmap[ref,'1']
    if name=='FRAM':
        sx,sy=dest['x']+p1['x'],dest['y']-p1['y']
        transform=lambda x,y:(sx-x,sy+y)
    else:
        sx,sy=dest['x']+p1['y'],dest['y']+p1['x']
        transform=lambda x,y:(sx-y,sy-x)
    for sp in source:
        dp=padmap[ref,sp['pin']];assert math.dist(transform(sp['x'],sp['y']),(dp['x'],dp['y']))<1e-6
    want=sorted(tuple(round(z,4) for z in transform(x,y)) for x,y in holes)
    got=sorted((round(p['x'],4),round(p['y'],4)) for p in native['pads'] if p['ref']==ref and p['npth'])
    assert want==got
    ada.append(dict(ref=ref,source_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),pads_checked=9,holes_checked=2,
                    coordinate_determinant=-1,meaning='y-up to y-down plus physical rotation; not a mirrored module',status='PASS'))

# Standard DIL24 pattern, clockwise global numbering, as shown in ST Fig2.
for pin in range(1,25):
    got=padmap['J8',str(pin)]
    x=45 if pin<=12 else 60.24;y=24+(pin-1 if pin<=12 else 24-pin)*2.54
    assert math.dist((got['x'],got['y']),(x,y))<1e-6

click=REPORTS/'TR2-shield-v26-20261105/Click2/RS485_Isolator_2_click-master/HW/RS485_Isolator_2_click_v101 - 2L/Project Files/RS485_Isolator_2_click_v101.PcbDoc'
cp=altium(click);headers=[p for p in cp if p['ref']=='MBD1']
assert len(headers)==16
click_pads=[dict(pin=p['pin'],native_net=p['net'],x_mm=p['x'],y_mm=p['y']) for p in headers]
# Geometry relative to pin1, matched in the physical90deg orientation, no reflection.
c1=next(p for p in headers if p['pin']=='1');dp1=padmap['J9','1']
click_deltas=[]
for p in headers:
    x=dp1['x']+(p['y']-c1['y']);y=dp1['y']+(p['x']-c1['x'])
    d=padmap['J9',p['pin']];click_deltas.append(math.dist((x,y),(d['x'],d['y'])))
assert max(click_deltas)<1e-6,max(click_deltas)
result=dict(ST_native_sha256=hashlib.sha256(st.read_bytes()).hexdigest(),ST_106_pad_comparison=comparison,
            ST_max_delta_mm=max(x['delta_mm'] for x in comparison),library_180_pads=lib_checks,
            Adafruit=ada,MKIGIBV2_DIL24_numbering='PASS geometry; FAIL electrical assignment remains',
            Click_v101_native_sha256=hashlib.sha256(click.read_bytes()).hexdigest(),Click_v101_headers=click_pads,
            Click_max_delta_mm=max(click_deltas),
            Click_v102_geometry='INCONNU exact native CAv102 unavailable; standard footprint and v102 schematic compared, not substituted by v101',
            physical_assembly='INCONNU; source geometry agreement does not prove contact, insertion depth or actual soldering')
(HERE/'orientations-et-empreintes.json').write_text(json.dumps(result,indent=2)+'\n')
print('ST106 pads delta',result['ST_max_delta_mm'],'; library180 pads; Adafruit18 pads+4holes; Click16 v101 pads; v102 physical remains unknown')
