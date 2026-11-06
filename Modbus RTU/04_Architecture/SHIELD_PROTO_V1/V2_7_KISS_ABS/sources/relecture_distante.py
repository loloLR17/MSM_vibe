"""After git fetch origin main, read every changed/baseline blob and remote ZIP."""
from pathlib import Path
import subprocess,json,hashlib,zipfile,io,sys,zlib
r=Path(subprocess.check_output(['git','rev-parse','--show-toplevel']).decode().strip());pre='Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_7_KISS_ABS';p=r/pre
remote=subprocess.check_output(['git','rev-parse','origin/main']).decode().strip();head=subprocess.check_output(['git','rev-parse','HEAD']).decode().strip();assert remote==head
initial=json.loads((p/'preuves/conservation-initiale.json').read_text());old=initial['files']
changed=list(filter(None,subprocess.check_output(['git','diff','--name-only','-z',initial['initial_head'],remote]).decode().split('\0')));names=sorted(set(old)|set(changed))
proc=subprocess.run(['git','cat-file','--batch'],input=('\n'.join(remote+':'+f for f in names)+'\n').encode(),stdout=subprocess.PIPE,check=True)
data=proc.stdout;i=0;hashes={};blobs={}
for name in names:
 end=data.index(b'\n',i);header=data[i:end].split();assert header[1]==b'blob',(name,header);size=int(header[2]);i=end+1;b=data[i:i+size];i+=size+1
 assert b==(r/name).read_bytes(),name
 h=hashlib.sha256(b).hexdigest();hashes[name]=h
 if name in old:assert h==old[name],name
 if name.startswith(pre):blobs[name[len(pre)+1:]]=b
assert i==len(data)
zpath='pack_Morvan/TR2_V27_ABS_POUR_MORVAN_2_PROTOTYPES.zip'
with zipfile.ZipFile(io.BytesIO(blobs[zpath]))as z:
 assert z.testzip()is None and len(z.namelist())==38
 man=json.loads(z.read('MANIFESTE.json'));assert man['mission_id']=='TR2-20261106-SHIELD-V27-KISS-ABS-MORVAN'
 assert man['STL_types']==25 and man['parts_two_prototypes']==61 and man['useful_parts']==46
 assert set(z.namelist())==set(man['files'])|{'MANIFESTE.json'}
 for name,meta in man['files'].items():
  b=z.read(name);assert b==blobs[name],name
  assert hashlib.sha256(b).hexdigest()==meta['sha256']and f'{zlib.crc32(b)&0xffffffff:08x}'==meta['crc32']and len(b)==meta['bytes'],name
 assert len([n for n in z.namelist()if n.endswith('.stl')])==25
assert 'mission_id: TR2-20261106-SHIELD-V27-KISS-ABS-MORVAN'in(r/'Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md').read_text()
result={'remote_sha':remote,'full_files_read_and_compared':len(names),'preserved_baseline_files':len(old),'changed_files_read':len(changed),'zip_sha256':hashlib.sha256(blobs[zpath]).hexdigest(),'zip_remote_members_verified':38,'STL_types':25,'parts_two_prototypes':61,'sha256_full_readback_manifest':hashlib.sha256(json.dumps(hashes,sort_keys=True).encode()).hexdigest(),'comparison':'all freshly fetched GitHub blobs byte-identical to local; all remote ZIP members CRC/SHA/size and individual remote source blobs compared'}
print(json.dumps(result,indent=2))
if len(sys.argv)>1:Path(sys.argv[1]).write_text(json.dumps(result,indent=2)+'\n')
