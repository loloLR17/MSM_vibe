"""Read-only forensic inspection of an archive REJECTED by the strict importer.
This tool NEVER returns COMPLETE/qualified. It independently checks the useful
bytes against a separate producer-RAM digest and reports the quality failure.
Unchanged read_campaign.verify remains the acceptance authority.
"""
from pathlib import Path
import struct,zlib,json,hashlib,argparse,csv
import read_campaign as strict

def inspect(path,ram,producer,output):
 try:strict.verify(path)
 except strict.Invalid as error:
  reason=str(error)
  if reason!='rail or clipping in completed capture':raise
 else:raise ValueError('Forensic quality-failure tool requires an actually rejected archive')
 if output.exists():raise FileExistsError(output)
 output.mkdir(parents=True)
 with path.open('rb') as stream:
  h=stream.read(512);stream.seek(-512,2);f=stream.read(512);stream.seek(512)
  assert h[:8]==b'TR2DUR01' and f[:8]==b'TR2END01' and strict.u32(h,8)==strict.u32(f,8)==3
  assert strict.u32(h,508)==zlib.crc32(h[:508]) and strict.u32(f,508)==zlib.crc32(f[:508])
  assert strict.u32(h,20)==strict.u32(f,44)==0 and h[176:508]==bytes(332) and f[152:508]==bytes(356)
  assert strict.u64(h,168)<=strict.u64(f,144)<=strict.u64(h,168)+5000 and strict.u64(f,144)==ram['duration_us']
  assert strict.u64(f,128)==strict.u32(f,88)+((strict.u64(f,128)>>32)<<32) and strict.u64(f,136)==strict.u32(f,92)+((strict.u64(f,136)>>32)<<32)
  session=strict.u64(h,152);assert session==strict.u64(f,16)
  assert struct.unpack_from('<4I',f,72)==(ram['warmup'],ram['rails'],ram['clips'],ram['pending_xyz'])
  assert strict.u32(f,28)==ram['outputs'] and strict.u32(f,32)==ram['accepted']
  assert strict.u32(f,40)==ram['sd_total_bytes'] and (ram['accepted']+9)//10==ram['outputs']
  assert (strict.u64(f,128),strict.u64(f,136))==(ram['first_us'],ram['last_us'])
  records=strict.u32(f,36);assert records*512==ram['sd_written'] and (records+2)*512<=path.stat().st_size
  count=0;last=None;first=None;digest=hashlib.sha256();flagged=[];quality_or=0;endpoints=[]
  for seq in range(records):
   b=stream.read(512);assert len(b)==512 and b[:4]==b'D3BK' and strict.u64(b,8)==session
   assert strict.u32(b,4)==seq and strict.u32(b,32)==count
   assert strict.u32(b,40)==zlib.crc32(b[:40]+bytes(4)+b[44:])
   n,quality=struct.unpack_from('<HH',b,36);start,end=struct.unpack_from('<QQ',b,16)
   assert 1<=n<=78 and (seq+1==records or n==78) and quality&~31==0 and not quality&24
   assert start<=end and (n==1 or start<end) and (last is None or start>last)
   assert b[44+6*n:]==bytes(468-6*n)
   if quality&6:
    candidates=[]
    for j in range(n):
     xyz=struct.unpack_from('<hhh',b,44+6*j)
     if any(v in (-32768,32767) for v in xyz):candidates.append(dict(output_index=count+j,source_index=(count+j)*10,xyz=list(xyz),interpolated_MCU_time_us=start+(end-start)*j//max(n-1,1),time_is_approximate=True))
    flagged.append(dict(record=seq,file_offset=(seq+1)*512,quality=quality,first_output=count,count=n,first_MCU_time_us=start,last_MCU_time_us=end,endpoint_candidates=candidates))
    (output/f'record-{seq}.bin').write_bytes(b)
   digest.update(b[44:44+6*n]);quality_or|=quality;count+=n;first=start if first is None else first;last=end
  assert count==ram['outputs'] and (first,last)==struct.unpack_from('<QQ',f,56)
  assert digest.digest()==producer==f[96:128]
  assert len([x for x in flagged if x['quality']&4])>=1 and ram['clips']>0
  for row in flagged:
   start=max(0,row['record']-2);end=min(records,row['record']+3);stream.seek((start+1)*512);data=stream.read((end-start)*512);(output/f'context-{row["record"]}.bin').write_bytes(data)
   with (output/f'context-{row["record"]}.csv').open('x',newline='') as target:
    writer=csv.writer(target);writer.writerow(['output_index','record_quality_aggregate','time_interpolated_MCU_us','x_count','y_count','z_count'])
    for off in range(0,len(data),512):
     b=data[off:off+512];n,q=struct.unpack_from('<HH',b,36);t0,t1=struct.unpack_from('<QQ',b,16);index=strict.u32(b,32)
     for j in range(n):writer.writerow([index+j,q,t0+(t1-t0)*j//max(n-1,1),*struct.unpack_from('<hhh',b,44+6*j)])
 result=dict(status='REJECTED_QUALITY',strict_importer_error=reason,measurement_qualified=False,declared_COMPLETE_is_not_accepted=True,declared_footer_complete=strict.u32(f,12),run=strict.u32(h,16),session=session,format_version=3,file_bytes=path.stat().st_size,firmware_sha=h[52:92].decode(),uid=list(struct.unpack_from('<3I',h,24)),cid=h[36:52].hex(),coefficient_sha256=h[92:124].hex(),requested_duration_us=strict.u64(h,168),actual_duration_us=strict.u64(f,144),valid_records=records,valid_xyz=count,payload_bytes=count*6,all_records_CRC_indices_timestamps_footer_counts_checked=True,all_useful_bytes_equal_independent_RAM_producer_SHA=True,payload_sha256=digest.hexdigest(),warmup=ram['warmup'],source_rails=ram['rails'],clipped_outputs=ram['clips'],pending_native=ram['pending_xyz'],quality_or=quality_or,first_time_us=first,last_time_us=last,first_source_time_us=ram['first_us'],last_source_time_us=ram['last_us'],flagged_records=flagged,scope='Storage byte integrity only; quality rejected, no change to strict importer or original bytes. Source raw samples at incident were not archived.')
 (output/'FORENSIC.json').write_text(json.dumps(result,indent=2)+'\n');return result
if __name__=='__main__':
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('file',type=Path);ap.add_argument('RAM_result',type=Path);ap.add_argument('producer_SHA',type=Path);ap.add_argument('output',type=Path);a=ap.parse_args();print(json.dumps(inspect(a.file,json.loads(a.RAM_result.read_text()),a.producer_SHA.read_bytes(),a.output),indent=2))
