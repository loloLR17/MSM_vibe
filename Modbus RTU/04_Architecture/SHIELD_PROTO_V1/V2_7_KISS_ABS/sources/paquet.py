"""Build the single Morvan ZIP; reopen every byte, SHA256 and CRC32."""
from pathlib import Path
import json,hashlib,zipfile,zlib
P=Path(__file__).resolve().parents[1];MID='TR2-20261106-SHIELD-V27-KISS-ABS-MORVAN'
def main():
 names=['README.md','IMPRIMEUR.md','MONTAGE_A_FROID.md','QUANTITES.csv','BOM_QUINCAILLERIE.csv','SOURCES.md','cad/eclate-un-prototype.step']
 names+=sorted(str(f.relative_to(P))for f in(P/'stl').glob('*.stl'))
 names+=sorted(str(f.relative_to(P))for f in(P/'vues').iterdir()if f.suffix.lower()in['.png','.pdf'])
 data={n:(P/n).read_bytes()for n in names}
 files={n:{'sha256':hashlib.sha256(b).hexdigest(),'crc32':f'{zlib.crc32(b)&0xffffffff:08x}','bytes':len(b)}for n,b in data.items()}
 man={'mission_id':MID,'material':'ABS non conductive','STL_types':25,'parts_two_prototypes':61,'useful_parts':46,'alternatives':12,'spare_D126':2,'coupon':1,'review':'Independent ChatGPT review before handing to Morvan; cold first fit required','files':files,'manifest_self_hash':'Manifest is excluded from its own hash; whole ZIP SHA256 is provided externally'}
 zpath=P/'pack_Morvan/TR2_V27_ABS_POUR_MORVAN_2_PROTOTYPES.zip';zpath.parent.mkdir(exist_ok=True)
 with zipfile.ZipFile(zpath,'w',zipfile.ZIP_DEFLATED,compresslevel=9)as z:
  for n,b in data.items():z.writestr(n,b)
  z.writestr('MANIFESTE.json',json.dumps(man,indent=2,ensure_ascii=False)+'\n')
 with zipfile.ZipFile(zpath)as z:
  assert z.testzip()is None and len(z.namelist())==len(data)+1
  for n,b in data.items():
   got=z.read(n);assert got==b and z.getinfo(n).CRC==zlib.crc32(b)&0xffffffff,n
 assert len([n for n in names if n.endswith('.stl')])==25
 h=hashlib.sha256(zpath.read_bytes()).hexdigest();zpath.with_suffix('.zip.sha256').write_text(h+'  '+zpath.name+'\n')
 proof={'mission_id':MID,'zip':str(zpath.relative_to(P)),'sha256':h,'bytes':zpath.stat().st_size,'entries':len(data)+1,'STL_types':25,'parts_two_prototypes':61,'CRC_full_readback':True,'all_members_byte_identical':True,'internal_manifest_files':files}
 (P/'preuves/paquet.json').write_text(json.dumps(proof,indent=2,ensure_ascii=False)+'\n')
 print(json.dumps({k:v for k,v in proof.items()if k!='internal_manifest_files'},indent=2))
if __name__=='__main__':main()
