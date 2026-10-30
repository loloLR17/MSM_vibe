"""A rejected quality fixture stays rejected; forensic byte proof is separate."""
from pathlib import Path
import importlib.util,sys,tempfile,struct,zlib,hashlib,json
root=Path(__file__).parents[1];sys.path.insert(0,str(root/'tools'));import read_campaign as strict;import inspect_quality_failure as forensic
b=Path(sys.argv[1]).read_bytes();h=bytearray(b[:512]);record=bytearray(b[512:1024]);f=bytearray(b[-512:]);n=78
struct.pack_into('<II',h,8,3,512);struct.pack_into('<I',h,20,0);struct.pack_into('<Q',h,168,2000000)
struct.pack_into('<HH',record,36,78,5);struct.pack_into('<h',record,44+10*6,32767)
struct.pack_into('<I',record,40,0);struct.pack_into('<I',record,40,zlib.crc32(record));digest=hashlib.sha256(record[44:44+6*n]).digest()
struct.pack_into('<I',f,8,3);struct.pack_into('<I',f,44,0);struct.pack_into('<QQ',f,128,5000000,5029212);struct.pack_into('<Q',f,144,2000000);struct.pack_into('<II',f,88,5000000,5029212);struct.pack_into('<III',f,72,11,0,1);f[96:128]=digest
ram=dict(duration_us=2000000,warmup=11,rails=0,clips=1,pending_xyz=0,outputs=78,accepted=780,sd_total_bytes=468,first_us=5000000,last_us=5029212,sd_written=512)
def crc(x):struct.pack_into('<I',x,508,zlib.crc32(x[:508]))
crc(h);crc(f)
with tempfile.TemporaryDirectory() as directory:
 d=Path(directory);p=d/'quality.TR2';p.write_bytes(h+record+bytes(512)+f)
 r=forensic.inspect(p,ram,digest,d/'inspection');assert r['status']=='REJECTED_QUALITY' and not r['measurement_qualified'] and r['all_useful_bytes_equal_independent_RAM_producer_SHA']
 assert len(r['flagged_records'])==1 and r['flagged_records'][0]['endpoint_candidates'][0]['output_index']==10
 try:forensic.inspect(p,ram,bytes(32),d/'wrong-digest')
 except AssertionError:pass
 else:raise AssertionError('Wrong independent digest accepted')
 # Quality exception is raised before v3 tail guards by strict reader; forensic
 # inspector must still check these guards, not silently accept the bypass.
 f[152]=1;crc(f);p.write_bytes(h+record+bytes(512)+f)
 try:forensic.inspect(p,ram,digest,d/'bad-reserved')
 except AssertionError:pass
 else:raise AssertionError('Unknown reserved footer field accepted')
 f[152]=0;crc(f);bad=record[:];bad[100]^=1;p.write_bytes(h+bad+bytes(512)+f)
 try:forensic.inspect(p,ram,digest,d/'bad-crc')
 except strict.Invalid:pass
 else:raise AssertionError('Bad record CRC accepted')
 # Desired future writer behavior: after data drain, no success footer for
 # clipped/railed samples; original data remain a readable INCOMPLETE prefix.
 p.write_bytes(h+record+bytes(1024));r=strict.verify(p);assert r['status']=='INCOMPLETE' and r['quality_or']==5 and r['sha256']==digest.hex()
print(json.dumps(dict(quality_remains_rejected=True,forensic_only=True,bad_digest_reserved_CRC_refused=True,incomplete_prefix_preserved=True,host_only=True)))
