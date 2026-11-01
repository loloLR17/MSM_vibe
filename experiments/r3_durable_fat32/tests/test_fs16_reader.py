"""Small synthetic profile fixtures, never a physical/endurance assertion."""
import hashlib,importlib.util,json,struct,tempfile,zlib
from pathlib import Path
spec=importlib.util.spec_from_file_location('reader',Path(__file__).parents[1]/'tools/read_campaign.py');reader=importlib.util.module_from_spec(spec);spec.loader.exec_module(reader)
def put(b,o,v,fmt='I'):struct.pack_into('<'+fmt,b,o,v)
def crc(b):put(b,508,zlib.crc32(b[:508]))
h=bytearray(512);h[:8]=b'TR2DUR01';h[52:92]=b'a'*40
for o,v in [(8,3),(12,512),(16,5),(124,8),(128,244),(132,10),(160,78)]:put(h,o,v)
for o,v in [(136,80000),(144,3),(152,123),(168,3000000)]:put(h,o,v,'Q')
r=bytearray(512);r[:4]=b'D3BK';put(r,8,123,'Q');put(r,16,5000000,'Q');put(r,24,5028875,'Q');put(r,36,78,'H')
payload=b''.join(struct.pack('<hhh',1000+i,-500,23) for i in range(78));r[44:]=payload;put(r,40,zlib.crc32(r))
f=bytearray(512);f[:8]=b'TR2END01'
for o,v in [(8,3),(12,1),(24,5),(28,78),(32,780),(36,1),(40,468),(48,1),(88,5000000),(92,5029212)]:put(f,o,v)
for o,v in [(16,123),(56,5000000),(64,5028875),(128,5000000),(136,5029212),(144,3000029)]:put(f,o,v,'Q')
f[96:128]=hashlib.sha256(payload).digest();crc(f)
# Exhaustive int16 conversion regression for the old FS8 nominal CSV scale.
for count in range(-32768,32768):assert format(count*.000244,'.6f')==format(count*(244/1000000),'.6f')
cases=0
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'fixture.TR2'
 for fs,ug,valid in [(8,244,True),(16,488,True),(8,488,False),(16,244,False),(2,61,False)]:
  put(h,124,fs);put(h,128,ug);crc(h);p.write_bytes(h+r+bytes(512)+f)
  try:result=reader.verify(p)
  except reader.Invalid:assert not valid
  else:
   assert valid and result['status']=='COMPLETE' and result['logical_fs_g']==fs
   row=next(reader.samples(p));assert row['x_nominal_g']==('0.244000' if fs==8 else '0.488000')
   assert result['sha256']==hashlib.sha256(payload).hexdigest()
  cases+=1
 # Version1 cannot silently acquire a new FS16 interpretation.
 put(h,124,16);put(h,128,488);put(h,8,1);crc(h);p.write_bytes(h+r+bytes(512)+f)
 try:reader.verify(p)
 except reader.Invalid:cases+=1
 else:raise AssertionError('legacy version accepted FS16')
 put(h,8,3)
 # CRC and quality guards remain strict for FS16.
 put(h,124,16);put(h,128,488);crc(h)
 for offset in [100,512+44,len(h+r+bytes(512)+f)-4]:
  b=bytearray(h+r+bytes(512)+f);b[offset]^=1;p.write_bytes(b)
  try:reader.verify(p)
  except reader.Invalid:cases+=1
  else:raise AssertionError('corruption accepted')
print(json.dumps(dict(profile_and_corruption_cases=cases,old8g_retained=True,fs16_scale_correct=True,synthetic_only=True)))
