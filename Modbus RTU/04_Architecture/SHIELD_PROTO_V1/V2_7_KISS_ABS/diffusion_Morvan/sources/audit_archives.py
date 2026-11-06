"""Read and really extract the existing V26/V27 ZIPs without changing them."""
from pathlib import Path,PurePosixPath
import json,hashlib,zipfile,re,csv
from donnees import *
EXTERNAL=Path('/home/lolo/dev/msm/reports/TR2-shield-v27-pack-morvan-pdf-20261106')
def obsolete(n):return bool(re.search(r'trappe|(?:^|[/_ -])cale(?:[_ .-]|$)|cassette[_ -](?:sud|sd)',n,re.I))
def read_extract(path,label):
 b=path.read_bytes();dest=EXTERNAL/label;dest.mkdir(parents=True,exist_ok=True);rows=[]
 with zipfile.ZipFile(path)as z:
  assert z.testzip()is None
  for info in z.infolist():
   n=info.filename;pure=PurePosixPath(n);assert not pure.is_absolute()and '..'not in pure.parts and not info.is_dir()
   data=z.read(n);assert len(data)==info.file_size
   z.extract(info,dest)
   assert(dest/n).read_bytes()==data
   rows.append({'entry':n,'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest(),'crc32':f'{info.CRC:08x}','obsolete_name':obsolete(n)})
 return {'path':str(path.relative_to(SHIELD)),'sha256':hashlib.sha256(b).hexdigest(),'entries':rows,'real_extraction':str(dest),'CRC_full_readback':True}
def main():
 EXTERNAL.mkdir(parents=True,exist_ok=True)
 old26=read_extract(SHIELD/'V2_6_IMPRESSION_COMPLETE/pack_imprimeur/TR2_V26_COMPLET_2_PROTOTYPES.zip','ancien_V26')
 old27=read_extract(OLD_ZIP,'technique_V27');assert old27['sha256']==OLD_SHA and len(old27['entries'])==38
 with zipfile.ZipFile(OLD_ZIP)as z:
  manifest=json.loads(z.read('MANIFESTE.json'));assert manifest['parts_two_prototypes']==61
  for n,meta in manifest['files'].items():assert hashlib.sha256(z.read(n)).hexdigest()==meta['sha256']
  stl={Path(r['entry']).name:r for r in old27['entries']if r['entry'].endswith('.stl')}
  assert set(stl)=={n for n,_,_ in WHITELIST}
  sources={}
  for n,q,role in WHITELIST:
   data=(V27/'stl'/n).read_bytes();h=hashlib.sha256(data).hexdigest();assert h==stl[n]['sha256']
   sources[n]={'quantity_two_prototypes':q,'role':role,'source':'V2_7_KISS_ABS/stl/'+n,'sha256':h,'bytes':len(data),'matches_old_V27_zip':True}
 assert not any(r['obsolete_name']for r in old27['entries'])
 names26=[r['entry']for r in old26['entries']if r['obsolete_name']]
 assert any('trappe_USER'in n for n in names26)and any('cale_'in n for n in names26)
 csvrows=list(csv.DictReader((V27/'QUANTITES.csv').open()))
 assert {Path(r['fichier_STL']).name:int(r['a_imprimer_deux_TR2'])for r in csvrows}=={n:q for n,q,_ in WHITELIST}
 proof={'mission_id':MID,'V26':old26,'V27_technical':old27,'V26_obsolete_entries':names26,'V27_obsolete_entries':[],'sources_frozen_V27':sources,'old_V27_contains_trappe_USER':False,'old_V27_contains_cale':False,'conclusion':'The reported names are really present in V26. Binary V27 contains none; the locally consulted user download cannot be identified from repository evidence alone.','passed':True}
 (P/'preuves/audit-archives-initiales.json').write_text(json.dumps(proof,indent=2,ensure_ascii=False)+'\n')
 lines=['# Audit binaire des anciens ZIP','',f'V2.6 :{len(old26["entries"])} entrées réellement décompressées ; SHA `{old26["sha256"]}`.',f'V2.7 technique :**38 entrées /25 STL**, réellement décompressées ; SHA `{old27["sha256"]}`.','',"**Le ZIP V2.7 ne contient ni `trappe_USER`, ni `cale_*`, ni cassette sud/SD obsolète.** Les noms signalés existent réellement dans le ZIP V2.6 ci-dessous. Nous ne disposons pas du téléchargement consulté par l’utilisateur ; son origine exacte n’est donc pas démontrée. Ce constat ne présume aucune erreur d’utilisateur.",'','## Noms obsolètes réellement trouvés dans V2.6','']
 lines+=['- `'+n+'`'for n in names26]
 lines+=['','## Liste blanche V2.7 vérifiée sur les octets','',"25 modèles différents,61 exemplaires **pour les deux prototypes ensemble**. Un STL représente une forme, pas la quantité à imprimer ; ne pas multiplier le lot par deux.",'','| STL | Rôle | Copies pour deux TR2 | Source gelée | Conforme |','|---|---|---:|---|---|']
 for n,q,role in WHITELIST:lines.append(f'| `{n}` | {role} |{q}| `V2_7_KISS_ABS/stl/{n}` | SHA/taille identiques au ZIP technique |')
 lines+=['','Total61 =46 utiles +12 alternatives +2 D126 de réserve +1 coupon. Les61 exemplaires ne deviennent pas61 fichiers ;25 fichiers ne signifient pas2×25 impressions.','',"Le nouvel envoi ne réemploie aucune archive ancienne :seuls les25 fichiers de cette liste blanche seront copiés, avec deux nouveaux PDF. Les dossiers historiques restent présents dans Git pour traçabilité, hors de toute diffusion à Morvan."]
 (P/'preuves/INVENTAIRE_ET_COMPARAISON.md').write_text('\n'.join(lines)+'\n')
 print(json.dumps({'V26_entries':len(old26['entries']),'V26_obsolete':names26,'V27_entries':len(old27['entries']),'V27_STL':len(sources),'V27_obsolete':[],'passed':True},indent=2))
if __name__=='__main__':main()
