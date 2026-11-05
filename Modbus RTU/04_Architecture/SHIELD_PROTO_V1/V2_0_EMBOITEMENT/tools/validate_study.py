from pathlib import Path
import json,hashlib,ast,subprocess
import cadquery as cq
import pymupdf as pdf
P=Path(__file__).resolve().parents[1];checks=[]
for f in (P/'cad').glob('*.step'):
 b=f.read_bytes();t=('\n'.join(x.rstrip()for x in b.decode().splitlines())+'\n').encode();assert b.split()==t.split();f.write_bytes(t);s=cq.importers.importStep(str(f)).val();assert s.isValid();checks.append({'file':str(f.relative_to(P)),'solids':len(s.Solids()),'valid':True,'sha256':hashlib.sha256(t).hexdigest(),'tokens_preserved':True})
for f in P.rglob('*'):
 if f.is_file()and f.suffix in ['.py','.md','.json','.log']:
  t=f.read_text();f.write_text('\n'.join(x.rstrip()for x in t.splitlines())+'\n')
for f in P.rglob('*.py'):ast.parse(f.read_text())
for f in P.rglob('*.json'):json.loads(f.read_text())
r=json.loads((P/'preuves/C05-collisions-G12p25.json').read_text());assert len(r['source_geometry'])==1060 and not r['intersections']and not r['module_retainer_positive_intersections'];assert not json.loads((P/'preuves/SD4682-contre-retenues.json').read_text())['positive_intersections']
with pdf.open(P/'cad/assemblage-et-coupes.pdf')as d:assert len(d)==2 and all(p.get_text() for p in d)
result={'status':'PASS_BOUNDED_CAD_STUDY','CAD_readback':checks,'source_ST_solids':1060,'full_source_unwanted_intersections_under_shield_at_G12p25':0,'retainer_vs_socket_and_module_envelope_intersections':0,'SD_manufacturer_vs_retainers_intersections':0,'horizontal_optical_rays':'3 obstructed, not a full observer cone or proof of invisibility','PDF':'2 pages reopened text checked; Z envelope view visually inspected','script_metadata_incident':'source_geometry list shadowed by socket rows, repaired from identical first ST inspection; Boolean results unchanged; final script corrected','XY_electrical_tests':'reused V19; unchanged CAD/firmware, no repeated ERC/DRC/build/hardware','printed':False,'purchased':False,'hardware_access':False,'physical_gate':'BLOCKED_NOT_TESTED','fabrication':'NO_GO','complete_worst_case_guarantee':False}
(P/'preuves/validation-finale.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
