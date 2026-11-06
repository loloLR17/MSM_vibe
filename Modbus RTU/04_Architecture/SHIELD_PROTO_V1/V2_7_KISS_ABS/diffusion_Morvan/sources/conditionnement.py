"""Build only the explicit 25-STL whitelist plus exactly two PDFs."""
import json,zipfile,hashlib,zlib
from donnees import *

def main():
 proof=json.loads((P/'preuves/audit-archives-initiales.json').read_text());data={}
 for n,q,role in WHITELIST:
  b=(V27/'stl'/n).read_bytes();ref=proof['sources_frozen_V27'][n]
  assert len(b)==ref['bytes']and hashlib.sha256(b).hexdigest()==ref['sha256']
  data['STL/'+n]=b
 for n in PDFS:data[n]=(P/'pdf'/n).read_bytes()
 assert len(data)==27 and sum(n.endswith('.stl')for n in data)==25
 out=V27/'pack_Morvan'/ZIP_NAME
 with zipfile.ZipFile(out,'w',zipfile.ZIP_DEFLATED,compresslevel=9)as z:
  for n,b in data.items():
   info=zipfile.ZipInfo(n,date_time=(2026,11,6,11,42,0));info.compress_type=zipfile.ZIP_DEFLATED;info.external_attr=0o100644<<16;z.writestr(info,b,compresslevel=9)
 with zipfile.ZipFile(out)as z:
  assert len(z.infolist())==27 and z.testzip()is None
  for n,b in data.items():assert z.read(n)==b and z.getinfo(n).CRC==(zlib.crc32(b)&0xffffffff)
 result={'mission_id':MID,'ZIP':str(out.relative_to(V27)),'sha256':hashlib.sha256(out.read_bytes()).hexdigest(),'bytes':out.stat().st_size,'entries':27,'STL':25,'PDF':2,'parts_two_prototypes':61,'builder_byte_comparison':True,'members':{n:{'sha256':hashlib.sha256(b).hexdigest(),'bytes':len(b),'crc32':f'{zlib.crc32(b)&0xffffffff:08x}'}for n,b in data.items()}}
 (P/'preuves/conditionnement.json').write_text(json.dumps(result,indent=2,ensure_ascii=False)+'\n');out.with_suffix('.zip.sha256').write_text(result['sha256']+'  '+out.name+'\n')
 print(json.dumps({k:v for k,v in result.items()if k!='members'},indent=2))
if __name__=='__main__':main()
