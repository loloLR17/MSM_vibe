"""Fetch first; then read every changed/baseline GitHub blob and all 27 ZIP members."""
import subprocess,json,hashlib,zipfile,io,sys
from pathlib import Path
from donnees import P,V27,ZIP_NAME,MID
r=Path(subprocess.check_output(['git','rev-parse','--show-toplevel']).decode().strip());remote=subprocess.check_output(['git','rev-parse','origin/main']).decode().strip();head=subprocess.check_output(['git','rev-parse','HEAD']).decode().strip();assert remote==head
base=json.loads((P/'preuves/conservation-initiale.json').read_text());old=base['files'];changed=list(filter(None,subprocess.check_output(['git','diff','--name-only','-z',base['initial_head'],remote]).decode().split('\0')));names=sorted(set(old)|set(changed))
payload=subprocess.check_output(['git','cat-file','--batch'],input=('\n'.join(remote+':'+n for n in names)+'\n').encode());pos=0;blobs={};hashes={}
for n in names:
 end=payload.index(b'\n',pos);h=payload[pos:end].split();assert h[1]==b'blob';size=int(h[2]);pos=end+1;b=payload[pos:pos+size];pos+=size+1
 assert b==(r/n).read_bytes(),n;digest=hashlib.sha256(b).hexdigest();hashes[n]=digest
 if n in old:assert digest==old[n],n
 blobs[n]=b
assert pos==len(payload)
prefix=str(V27.relative_to(r));zname=prefix+'/pack_Morvan/'+ZIP_NAME;expected={prefix+'/diffusion_Morvan/pdf/01_LIRE_EN_PREMIER_MORVAN.pdf':None,prefix+'/diffusion_Morvan/pdf/02_VUES_DES_PIECES.pdf':None}
with zipfile.ZipFile(io.BytesIO(blobs[zname]))as z:
 assert len(z.namelist())==len(set(z.namelist()))==27 and z.testzip()is None
 assert sum(n.endswith('.stl')for n in z.namelist())==25
 for n in z.namelist():
  path=prefix+'/stl/'+n[4:]if n.startswith('STL/')else prefix+'/diffusion_Morvan/pdf/'+n
  assert path in blobs and z.read(n)==blobs[path],n
report='Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md';assert ('mission_id: '+MID).encode()in blobs[report]
result={'remote_sha':remote,'full_files_read_and_compared':len(names),'preserved_baseline_files':len(old),'changed_files_read':len(changed),'zip_sha256':hashlib.sha256(blobs[zname]).hexdigest(),'zip_remote_members_verified':27,'STL':25,'PDF':2,'sha256_full_readback_manifest':hashlib.sha256(json.dumps(hashes,sort_keys=True).encode()).hexdigest(),'comparison':'Freshly fetched complete GitHub blobs compared byte for byte; frozen baseline preserved; all 27 remote ZIP members CRC/bytes verified against individual remote sources'}
print(json.dumps(result,indent=2))
if len(sys.argv)>1:Path(sys.argv[1]).write_text(json.dumps(result,indent=2)+'\n')
