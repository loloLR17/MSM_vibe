"""New export/document checks only; do not repeat unchanged BREP/mesh tests."""
from pathlib import Path
import json,hashlib,csv,subprocess,ast
import cadquery as cq
import fitz
P=Path(__file__).resolve().parents[1]
rows=json.loads((P/'preuves/pieces.json').read_text());proof={}
for f in (P/'sources').glob('*.py'):ast.parse(f.read_text(),filename=str(f))
for r in rows:assert hashlib.sha256((P/r['STL']).read_bytes()).hexdigest()==r['STL_sha256'],r['id']
proof['unchanged_STL_sha256']=len(rows)
proof['STEP_reopened']={}
for n in ['assemblage-propre.step','quincaillerie-enveloppes.step','assemblage-complet-enveloppes.step','eclate-un-prototype.step']:
 s=cq.importers.importStep(str(P/'cad'/n)).val();solids=s.Solids();assert solids and all(q.isValid()and q.Volume()>0 for q in solids),n
 proof['STEP_reopened'][n]={'solids':len(solids),'all_solids_valid':True,'bytes':(P/'cad'/n).stat().st_size}
 print('STEP reopen',n,len(solids),flush=True)
assert proof['STEP_reopened']['assemblage-propre.step']['solids']==23
with (P/'QUANTITES.csv').open(newline='')as f:q=list(csv.DictReader(f))
assert len(q)==25 and sum(int(r['a_imprimer_deux_TR2'])for r in q)==61
assert sum(int(r['utilise_deux_TR2_exemple_H11p2'])for r in q)==46
assert all((P/r['fichier_STL']).exists()for r in q)
proof['counts']={'STL_types':25,'total':61,'useful':46,'alternatives':12,'D126_spares':2,'coupon':1,'previous':142,'reduction':81,'reduction_percent':81/142*100}
doc=fitz.open(P/'vues/PLAN_MONTAGE_V27.pdf');assert len(doc)==6
pages=[]
for i,page in enumerate(doc):
 text=page.get_text();assert text or page.get_images(),i
 # Text blocks must be inside the PDF page; images checked by visual review.
 for b in page.get_text('blocks'):
  assert b[0]>=-.5 and b[1]>=-.5 and b[2]<=page.rect.width+.5 and b[3]<=page.rect.height+.5,(i,b)
 pages.append({'page':i+1,'width_pt':page.rect.width,'height_pt':page.rect.height,'text_chars':len(text),'images':len(page.get_images())})
proof['PDF_pages']=pages
base=json.loads((P/'preuves/conservation-initiale.json').read_text());root=P.parents[3]
# Repository root derived explicitly rather than from an assumed number of parents.
root=Path(subprocess.check_output(['git','rev-parse','--show-toplevel'],cwd=P).decode().strip())
for f,h in base['files'].items():assert hashlib.sha256((root/f).read_bytes()).hexdigest()==h,f
proof['preserved_baseline_files']=len(base['files']);proof['physical_qualification']=False;proof['passed']=True
(P/'preuves/qualite-livraison.json').write_text(json.dumps(proof,indent=2,ensure_ascii=False)+'\n')
print(json.dumps(proof,indent=2,ensure_ascii=False),flush=True)
