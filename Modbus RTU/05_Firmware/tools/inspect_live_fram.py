#!/usr/bin/env python3
"""Offline R0 FRAM auditor. Reads bytes only; no hardware or write capability."""
import argparse, hashlib, json, struct, zlib
from pathlib import Path

def u16(b,o): return struct.unpack_from('>H',b,o)[0]
def u32(b,o): return struct.unpack_from('>I',b,o)[0]
def u64(b,o): return struct.unpack_from('>Q',b,o)[0]
def sha(b): return hashlib.sha256(b).hexdigest()
def record(b,at,n,magic,envelope=False):
 p=b[at:at+n]
 if not any(p) or all(x==255 for x in p): return {'at':at,'empty':True}
 assert len(p)==n and p[:4]==magic and u32(p,n-4)==zlib.crc32(p[:-4])
 r={'at':at,'empty':False,'magic':magic.decode(),'generation':u64(p,8),'crc32':f'{u32(p,n-4):08x}','crc_valid':True,'sha256':sha(p)}
 r.update({'physical_version':u16(p,4),'logical_version':u16(p,6)} if envelope else {'format':u16(p,4),'size':u16(p,6)})
 return r
def inspect(raw):
 assert len(raw)==262144
 result={'bytes':len(raw),'sha256':sha(raw),'partitions':{name:{'start':a,'end':b,'sha256':sha(raw[a:b])} for name,a,b in [('historical',0,122880),('LIVE',122880,245760),('tail',245760,262144)]}}
 roots=[record(raw,at,64,b'TR2M',True) for at in (122880,126976)]
 root=max((x for x in roots if not x['empty']),key=lambda x:x['generation']);at=root['at'];assert raw[at+16] in (0,1) and u16(raw,at+4)==1 and u32(raw,at+20)==52298 and u32(raw,at+24)==64;bank=131072 if raw[at+16]==0 else 188416
 header=record(raw,bank,64,b'TR2I',True);assert u16(raw,at+6)==3 and header['generation']==root['generation'] and u16(raw,bank+6)==3 and u32(raw,bank+16)==52298
 image=raw[bank+64:bank+64+52298];assert zlib.crc32(image)==u32(raw,bank+24)
 result.update(roots=roots,active_root=root,active_header=header,active_payload_sha256=sha(image),payload_crc_valid=True,journal=[])
 for slot in range(128):
  copies=[]
  for copy in range(2):
   at=15922+224*slot+112*copy;r=record(image,at,112,b'TR2J')
   if r['empty']:continue
   assert r['format']==4 and r['size']==112
   r.update(copy=copy,epoch=u32(image,at+16),txid=u16(image,at+20),command=u16(image,at+22),phase=u16(image,at+34),status=u16(image,at+36),result=u16(image,at+38),run_id=u64(image,at+48),config_id=u32(image,at+56),config_generation=u32(image,at+60),effect_state=u16(image,at+64));copies.append(r)
  if copies:result['journal'].append({'slot':slot,'copies':copies,'selected_generation':max(r['generation'] for r in copies)})
 for name,base,magic in [('epoch',51914,b'TR2Q'),('intent_or_effect',52042,b'TR2E'),('provenance',52170,b'TR2M')]:
  pair=[]
  for copy in range(2):
   at=base+64*copy;r=record(image,at,64,magic)
   if not r['empty']:
    if name=='epoch':r.update(current=u32(image,at+16),previous=u32(image,at+24),flags=u16(image,at+30),transition=u64(image,at+32))
    if name=='intent_or_effect':r.update(epoch=u32(image,at+16),txid=u16(image,at+20),command=u16(image,at+22),run_id=u64(image,at+24),config_id=u32(image,at+32),config_generation=u32(image,at+36),phase=u16(image,at+40))
    if name=='provenance':r['historical_payload_sha256']=image[at+16:at+48].hex()
   pair.append(r)
  result[name]=pair
 return result
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('snapshot',type=Path);p.add_argument('--expect-sha');a=p.parse_args()
 raw=a.snapshot.read_bytes();raw=zlib.decompress(raw) if a.snapshot.suffix=='.zlib' else raw
 if a.expect_sha:assert sha(raw)==a.expect_sha
 print(json.dumps(inspect(raw),indent=2))
