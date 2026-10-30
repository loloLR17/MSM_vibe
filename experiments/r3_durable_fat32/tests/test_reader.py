import csv, hashlib, importlib.util, json, struct, sys, tempfile, zlib
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
 c=bytearray(b);struct.pack_into('<I',c,8,99);crc(c,508);check(c,'CORRUPT')
 # Known CRC but invalid index/timestamp/version are still refused.
 for off,value,fmt in [(516,9,'I'),(544,1,'I'),(528,5029000,'Q'),(550,8,'H')]:
  c=bytearray(b);struct.pack_into('<'+fmt,c,off,value);struct.pack_into('<I',c,552,0);struct.pack_into('<I',c,552,zlib.crc32(c[512:1024]));check(c,'CORRUPT')
 c=bytearray(b);struct.pack_into('<I',c,len(c)-512+40,467);crc(c,len(c)-4,len(c)-512);check(c,'CORRUPT')
 # Format2 beyond the absolute uint32 microsecond boundary; source/output
 # endpoints remain exact, with low32 fields explicitly cross-checked.
 c=bytearray(b);struct.pack_into('<I',c,8,2);struct.pack_into('<I',c,20,3600000000);crc(c,508)
 t=(1<<32)+5000000
 struct.pack_into('<QQ',c,528,t,t+28875);struct.pack_into('<I',c,552,0);struct.pack_into('<I',c,552,zlib.crc32(c[512:1024]))
 f=len(c)-512;struct.pack_into('<I',c,f+8,2);struct.pack_into('<I',c,f+44,3600000000);struct.pack_into('<QQ',c,f+56,t,t+28875)
 struct.pack_into('<II',c,f+88,t&0xffffffff,(t+29212)&0xffffffff);struct.pack_into('<QQ',c,f+128,t,t+29212);crc(c,len(c)-4,f)
 check(c,'COMPLETE');p.write_bytes(c);v=m.verify(p);assert v['last_source_time_us']==t+29212 and v['requested_duration_us']==3600000000
 c[f+136]^=1;crc(c,len(c)-4,f);check(c,'CORRUPT')
 # Missing footer with valid header/data retains exact prefix, never COMPLETE.
 c=bytearray(b);c[-512:]=bytes(512);p.write_bytes(c);r=m.verify(p);assert r['status']=='INCOMPLETE' and r['valid_xyz']==78 and r['sha256']==hashlib.sha256(expected).hexdigest();count+=1
 # A torn data sector without commit is incomplete, not a fake success.
 c=bytearray(b);c[-512:]=bytes(512);c[512+100]^=1;p.write_bytes(c);r=m.verify(p);assert r['status']=='INCOMPLETE' and r['valid_xyz']==0 and 'tail' in r['stop_reason'];count+=1
 # Clock/quality reconstruction and portable XYZ extraction from C fixture.
 export=Path(d)/'export.csv';m.write_csv(fixture,export);rows=list(csv.DictReader(export.open()))
 assert m.verify(fixture)['observed_duration_us']==28875 and abs(m.verify(fixture)['observed_output_hz_between_endpoints']-8000/3)<1e-9
 assert len(rows)==78 and int(rows[0]['time_us_interpolated'])==5000000 and int(rows[-1]['time_us_interpolated'])==5028875
 assert [int(x['source_index']) for x in rows]==list(range(0,780,10))
 assert [int(x['x_count']) for x in rows]==list(range(1000,1078)) and sum(int(x['warmup']) for x in rows)==11
 assert rows[0]['x_nominal_g']=='0.244000' and rows[0]['y_count']=='-500';count+=1
print(json.dumps(dict(independent_C_to_Python=True,corruption_truncation_version_index_time_cases=count,exact_xyz=78,host_only=True)))
