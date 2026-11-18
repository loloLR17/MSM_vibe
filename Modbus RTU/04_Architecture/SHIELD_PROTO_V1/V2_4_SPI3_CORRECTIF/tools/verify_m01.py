"""Independent full native mechanical identity gate. No design.json/router input.

Permitted differences: ONLY six J8 net names, four SPI3 signal segment groups,
and derived zone fill polygons. Vias, all drills and every other native property
are exact. A new or shifted hole, text, footprint, pad, body or setup is a FAIL.
"""
from pathlib import Path
from collections import Counter
import copy,json,hashlib
import sexpdata as sx
P=Path(__file__).resolve().parents[1];BASE=P.parent/'V2_3_ARNACO'
T=lambda n,t:isinstance(n,list)and bool(n)and str(n[0])==t
one=lambda n,t:next(x for x in n if T(x,t))
A=sx.loads((BASE/'cad/TR2_Shield_Proto_V1.kicad_pcb').read_text());B=sx.loads((P/'cad/TR2_Shield_Proto_V1.kicad_pcb').read_text())
expected={19:('NC','ACC_CS'),20:('NC','ACC_SCK'),21:('ACC_CS','ACC_MOSI'),22:('ACC_SCK','ACC_MISO'),23:('ACC_MOSI','NC'),24:('ACC_MISO','NC')}

def snapshots(root):
 mechanical=[];padnets={};holes=[];via=[];components={}
 for original in root[1:]:
  n=copy.deepcopy(original)
  if T(n,'segment')and one(n,'net')[1]in('ACC_CS','ACC_SCK','ACC_MISO','ACC_MOSI'):
   assert one(n,'layer')[1]in('F.Cu','B.Cu'), 'SPI3 track on non-copper layer'
   continue
  if T(n,'zone'):n[:]=[x for x in n if not T(x,'filled_polygon')]
  if T(n,'footprint'):
   ref=next(q[2]for q in n if T(q,'property')and q[1]=='Reference')
   components[ref]=dict(position=one(n,'at')[1:],layer=one(n,'layer')[1],library=n[1])
   for pad in n:
    if not T(pad,'pad'):continue
    net=next((q[1]for q in pad if T(q,'net')),'')
    if pad[1]:padnets[ref,str(pad[1])]=net
    holes.append(dict(ref=ref,pin=pad[1],pad_geometry=sx.dumps([q for q in pad if not T(q,'net') and not T(q,'uuid')]),footprint_at=components[ref]['position'],face=components[ref]['layer']))
    if ref=='J8' and pad[1] and int(pad[1])in expected:pad[:]=[q for q in pad if not T(q,'net')]
  if T(n,'via'):via.append(sx.dumps(n))
  mechanical.append(sx.dumps(n))
 return Counter(mechanical),padnets,holes,Counter(via),components

a,pa,ha,va,fa=snapshots(A);b,pb,hb,vb,fb=snapshots(B)
assert a==b,{'unexpected_native_removed':list((a-b).elements())[:3],'unexpected_native_added':list((b-a).elements())[:3]}
assert fa==fb and ha==hb and va==vb and sum(va.values())==4
norm=lambda net:'NC'if net.startswith('unconnected-')or not net else net
changes=[]
for key in pa:
 if pa[key]!=pb[key]:changes.append(dict(ref=key[0],pin=key[1],before=norm(pa[key]),after=norm(pb[key])))
assert len(changes)==6
assert {(q['ref'],int(q['pin'])):(q['before'],q['after'])for q in changes}=={('J8',k):v for k,v in expected.items()}
# Complete footprint library and mechanical/program rules identical bytes, too.
for path in (BASE/'cad').rglob('*'):
 if path.is_file()and path.suffix not in('.kicad_pcb','.kicad_sch','.kicad_sym','.json'):
  assert path.read_bytes()==(P/'cad'/path.relative_to(BASE/'cad')).read_bytes(),path
# Preserved V2.3 and V2.7 full-file fingerprints include all STL/STEP/PDF/quantities.
root=next(x for x in P.parents if(x/'AGENTS.md').is_file())
manifest=json.loads((P/'preuves/preservation-initiale.json').read_text())
for path,sha in manifest['files'].items():assert hashlib.sha256((root/path).read_bytes()).hexdigest()==sha,path
result={'verdict':'PASS_NATIVE_M01_MECHANICAL_IDENTITY','source_sha256':hashlib.sha256((BASE/'cad/TR2_Shield_Proto_V1.kicad_pcb').read_bytes()).hexdigest(),'candidate_sha256':hashlib.sha256((P/'cad/TR2_Shield_Proto_V1.kicad_pcb').read_bytes()).hexdigest(),'footprints_exact':fa,'all_pad_and_hole_mechanics_exact':ha,'vias_exact':list(va.elements()),'changed_net_contacts':changes,'contour_stackup_thickness_all_other_native_nodes_exact':True,'all_silkscreen_ARNACO_and_graphics_exact':True,'native_properties_compared':sum(a.values()),'historical_files_hash_verified':len(manifest['files']),'zone_fill_excluded_from_geometry_comparison_only_as_derived_electrical_copper':True,'remaining_gate':'Read frozen V2.7 STEP/interfaces; native DRC/ERC and E01/CAM; independent ChatGPT review. Not a physical assembly qualification.'}
(P/'preuves/M01-native.json').write_text(json.dumps(result,indent=2,ensure_ascii=False)+'\n')
print('M01 PASS native:',len(fa),'footprints,',len(ha),'pad/hole geometries,',len(va),'vias; six J8 nets changed, all other properties exact')
