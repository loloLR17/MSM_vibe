"""Independent second check: derive expected files/counts from frozen Git, not builder whitelist."""
from pathlib import Path
import subprocess,json,csv,io,zipfile,hashlib,re,ast
import fitz
from donnees import P,V27,FROZEN,OLD_ZIP,OLD_SHA,ZIP_NAME,MID
ROOT=Path(subprocess.check_output(['git','rev-parse','--show-toplevel']).decode().strip());PRE=str(V27.relative_to(ROOT))
def gitblob(f):return subprocess.check_output(['git','show',FROZEN+':'+f])
def obsolete(s):return bool(re.search(r'trappe|(?:^|[/_ -])cale(?:[_ .-]|$)|cassette[_ -](?:sud|sd)',s,re.I))
def main():
 names=list(filter(None,subprocess.check_output(['git','ls-tree','-rz','--name-only',FROZEN,'--',PRE+'/stl/']).decode().split('\0')));assert len(names)==25
 expected={Path(f).name:gitblob(f)for f in names}
 rows=list(csv.DictReader(io.StringIO(gitblob(PRE+'/QUANTITES.csv').decode())));counts={Path(r['fichier_STL']).name:int(r['a_imprimer_deux_TR2'])for r in rows};assert set(counts)==set(expected)and sum(counts.values())==61
 path=V27/'pack_Morvan'/ZIP_NAME;members=[];pdfdata={}
 with zipfile.ZipFile(path)as z:
  assert z.testzip()is None and len(z.infolist())==27
  assert len(set(z.namelist()))==27
  assert set(z.namelist())=={'STL/'+n for n in expected}|{'01_LIRE_EN_PREMIER_MORVAN.pdf','02_VUES_DES_PIECES.pdf'}
  for info in z.infolist():
   n=info.filename;b=z.read(n);assert not info.is_dir()and not obsolete(n)
   assert Path(n).suffix.lower()in ['.stl','.pdf']and not b.startswith(b'PK\x03\x04')
   if n.startswith('STL/'):
    source=expected[Path(n).name];assert b==source and len(b)==len(source)
   else:pdfdata[n]=b;assert b==(P/'pdf'/n).read_bytes()
   members.append({'entry':n,'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest(),'crc32':f'{info.CRC:08x}','conforme':True})
 pdfchecks={};texts={}
 for name,b in pdfdata.items():
  d=fitz.open(stream=b,filetype='pdf');assert len(d)==(3 if name.startswith('01_')else 2);pageinfo=[];full=''
  for i,page in enumerate(d):
   txt=page.get_text();full+=txt+'\n';assert len(txt)>200 and '\ufffd'not in txt
   assert "TR2 V2.7 ABS — dossier d'impression Morvan — 2 prototypes"in' '.join(txt.split())
   assert 'aucun ancien ZIP V2.6'in txt
   for block in page.get_text('dict')['blocks']:
    if 'lines'not in block:continue
    for line in block['lines']:
     for span in line['spans']:
      box=span['bbox'];assert box[0]>=20 and box[1]>=10 and box[2]<page.rect.width-15 and box[3]<page.rect.height-10,(name,i,span)
   assert not obsolete(txt),(name,i)
   render=P/'rendus_pdf'/(name[:-4]+f'-page-{i+1}.png');page.get_pixmap(matrix=fitz.Matrix(1.7,1.7)).save(render)
   pageinfo.append({'page':i+1,'text_characters':len(txt),'render':str(render.relative_to(P)),'render_sha256':hashlib.sha256(render.read_bytes()).hexdigest()})
  texts[name]=full;pdfchecks[name]={'pages':len(d),'page_checks':pageinfo,'sha256':hashlib.sha256(b).hexdigest()};d.close()
 notice=fitz.open(stream=pdfdata['01_LIRE_EN_PREMIER_MORVAN.pdf'],filetype='pdf');checked=[]
 for n,q in counts.items():
  found=[]
  for page in notice:
   words=page.get_text('words');matches=[w for w in words if w[4]==n]
   for w in matches:
    nums=[t for t in words if 360<t[0]<405 and abs(t[1]-w[1])<4 and t[4]==str(q)]
    assert len(nums)==1,(n,q,nums);found.append(page.number+1)
  assert len(found)==1,(n,found)
  checked.append({'filename':n,'copies_for_two_TR2':q,'PDF_page':found[0],'actual_PDF_text_and_quantity_match_frozen_Git_CSV':True})
 nt=texts['01_LIRE_EN_PREMIER_MORVAN.pdf'];assert all(s in nt for s in ['électronique','pièces','échelle','réserve','échantillon'])
 assert '61' in nt and '46 utiles + 12 alternatives + 2 réserves D126 + 1 échantillon.'in nt
 assert 'pas de tri'in nt and 'Aucun achat'in nt
 assert hashlib.sha256(OLD_ZIP.read_bytes()).hexdigest()==OLD_SHA
 base=json.loads((P/'preuves/conservation-initiale.json').read_text())
 for f,h in base['files'].items():assert hashlib.sha256((ROOT/f).read_bytes()).hexdigest()==h,f
 for f in(P/'sources').glob('*.py'):ast.parse(f.read_text(),filename=str(f))
 result={'mission_id':MID,'frozen_source_sha':FROZEN,'ZIP':str(path.relative_to(V27)),'sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'bytes':path.stat().st_size,'entries':27,'STL':25,'PDF':2,'copies_two_prototypes':61,'counts':{'useful':46,'alternatives':12,'D126_spares':2,'coupon':1},'members':members,'PDF_checks':pdfchecks,'actual_PDF_quantity_rows':checked,'obsolete_or_nested_or_technical_entries':[],'CRC_full_readback':True,'independent_frozen_git_comparison':True,'old_technical_ZIP_unchanged':True,'preserved_historical_shield_files':len(base['files']),'PDF_visual_inspection':'separate rendered-page review: preuves/revue-visuelle.json','passed_automatic':True}
 (P/'preuves/validation-finale.json').write_text(json.dumps(result,indent=2,ensure_ascii=False)+'\n')
 print(json.dumps({k:v for k,v in result.items()if k not in ['members','PDF_checks','actual_PDF_quantity_rows']},indent=2))
if __name__=='__main__':main()
