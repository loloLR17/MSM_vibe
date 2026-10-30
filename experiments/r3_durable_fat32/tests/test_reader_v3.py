"""Serialization/boundary fixtures only, never eight hours of physical samples."""
import importlib.util,sys,struct,tempfile,zlib,json
from pathlib import Path
spec=importlib.util.spec_from_file_location('reader',Path(__file__).parents[1]/'tools/read_campaign.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
b=Path(sys.argv[1]).read_bytes();h=bytearray(b[:512]);record=bytearray(b[512:1024]);f=bytearray(b[-512:]);size=640*1024*1024;t=2**32+5000000
struct.pack_into('<I',h,8,3);struct.pack_into('<I',h,20,0);struct.pack_into('<Q',h,168,30600000000)
struct.pack_into('<QQ',record,16,t,t+28875);struct.pack_into('<I',record,40,0);struct.pack_into('<I',record,40,zlib.crc32(record))
struct.pack_into('<I',f,8,3);struct.pack_into('<I',f,44,0);struct.pack_into('<QQ',f,56,t,t+28875);struct.pack_into('<II',f,88,t&0xffffffff,(t+29212)&0xffffffff);struct.pack_into('<QQQ',f,128,t,t+29212,30600000029)
def crc(x):struct.pack_into('<I',x,508,zlib.crc32(x[:508]))
count=0
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'v3.TR2'
 def check(expected):
  global count
  crc(h);crc(f)
  with p.open('wb') as stream:stream.write(h);stream.write(record);stream.seek(size-512);stream.write(f)
  try:r=m.verify(p)
  except m.Invalid:assert expected=='CORRUPT';count+=1;return
  assert r['status']==expected
  if expected=='COMPLETE':assert r['actual_duration_us']==struct.unpack_from('<Q',f,144)[0] and r['requested_duration_us']==struct.unpack_from('<Q',h,168)[0]
  count+=1
 check('COMPLETE')
 for duration in [28800000000,30600000000,86400000000]:
  struct.pack_into('<Q',h,168,duration);struct.pack_into('<Q',f,144,duration+29);check('COMPLETE')
 struct.pack_into('<Q',h,168,30600000000);struct.pack_into('<Q',f,144,30600000029)
 for buf,off,fmt,value in [(h,20,'I',123),(f,44,'I',123),(h,168,'Q',0),(f,144,'Q',1),(f,144,'Q',30600005001),(h,176,'I',1),(f,152,'I',1),(h,8,'I',4),(f,8,'I',2)]:
  original=struct.unpack_from('<'+fmt,buf,off)[0];struct.pack_into('<'+fmt,buf,off,value);check('CORRUPT');struct.pack_into('<'+fmt,buf,off,original)
 # Invalid/missing completion retains only a verified prefix; no success invented.
 saved=f[:];f[:]=bytes(512);check('INCOMPLETE');f[:]=saved
print(json.dumps({'v3_u64_cases':count,'real_sparse_file_bytes':size,'payload_fixture_only_XYZ':78,'not_physical_8h':True}))
