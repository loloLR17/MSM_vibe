import hashlib, importlib.util, json, struct, sys, tempfile, zlib
from pathlib import Path
spec=importlib.util.spec_from_file_location('reader',Path(__file__).parents[1]/'tools/read_campaign.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
fixture=Path(sys.argv[1]);b=fixture.read_bytes();r=m.verify(fixture);assert r['status']=='COMPLETE' and r['valid_xyz']==78
expected=b''.join(struct.pack('<hhh',1000+i,-500,23) for i in range(78));assert r['sha256']==hashlib.sha256(expected).hexdigest() and b[556:1024]==expected
count=0
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'test.TR2'
 def check(data,status):
  global count
  p.write_bytes(data)
  try:r=m.verify(p)
  except m.Invalid:
   assert status=='CORRUPT';count+=1;return
  assert r['status']==status,(r,status);count+=1
 def crc(data,off,start=0):struct.pack_into('<I',data,off,zlib.crc32(data[start:off]))
 # Truncation at every boundary of preparation, record and footer. Header torn
 # has no interpretable profile; prefix recovered only from fully valid records.
 for n in [0,1,8,511]:check(b[:n],'CORRUPT')
 for n in [512,513,700,1023,1024,1536,len(b)-512]:check(b[:n],'INCOMPLETE')
 for off in [0,8,124,152,508,512+44,512+40,len(b)-512+96,len(b)-1]:
  c=bytearray(b);c[off]^=1;check(c,'CORRUPT')
 c=bytearray(b);struct.pack_into('<I',c,8,2);crc(c,508);check(c,'CORRUPT')
 # Known CRC but invalid index/timestamp/version are still refused.
 for off,value,fmt in [(516,9,'I'),(544,1,'I'),(528,5029000,'Q'),(550,8,'H')]:
  c=bytearray(b);struct.pack_into('<'+fmt,c,off,value);struct.pack_into('<I',c,552,0);struct.pack_into('<I',c,552,zlib.crc32(c[512:1024]));check(c,'CORRUPT')
 c=bytearray(b);struct.pack_into('<I',c,len(c)-512+40,467);crc(c,len(c)-4,len(c)-512);check(c,'CORRUPT')
 # Missing footer with valid header/data retains exact prefix, never COMPLETE.
 c=bytearray(b);c[-512:]=bytes(512);p.write_bytes(c);r=m.verify(p);assert r['status']=='INCOMPLETE' and r['valid_xyz']==78 and r['sha256']==hashlib.sha256(expected).hexdigest();count+=1
 # A torn data sector without commit is incomplete, not a fake success.
 c=bytearray(b);c[-512:]=bytes(512);c[512+100]^=1;p.write_bytes(c);r=m.verify(p);assert r['status']=='INCOMPLETE' and r['valid_xyz']==0 and 'tail' in r['stop_reason'];count+=1
print(json.dumps(dict(independent_C_to_Python=True,corruption_truncation_version_index_time_cases=count,exact_xyz=78,host_only=True)))
