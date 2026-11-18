"""Copy only derived copper fill polygons from native private KiCad fill output.
Preserve every other candidate expression exactly for the M01 comparison.
"""
from pathlib import Path
import json,sys,sexpdata as sx
P=Path(__file__).resolve().parents[1];f=P/'cad/TR2_Shield_Proto_V1.kicad_pcb'
T=lambda n,t:isinstance(n,list)and bool(n)and str(n[0])==t
one=lambda n,t:next(x for x in n if T(x,t))
a=sx.loads(f.read_text());b=sx.loads(Path(sys.argv[1]).read_text());zones={one(n,'uuid')[1]:n for n in b if T(n,'zone')};counts=[]
for z in a:
 if T(z,'zone'):
  new=zones[one(z,'uuid')[1]];fills=[x for x in new if T(x,'filled_polygon')];assert fills
  z[:]=[x for x in z if not T(x,'filled_polygon')]+fills
  counts.append({'uuid':one(z,'uuid')[1],'layer':one(z,'layer')[1],'fill_polygons':len(fills)})
f.write_text(sx.dumps(a)+'\n');(P/'preuves/zone-fills.json').write_text(json.dumps(counts,indent=2)+'\n');print(counts)
