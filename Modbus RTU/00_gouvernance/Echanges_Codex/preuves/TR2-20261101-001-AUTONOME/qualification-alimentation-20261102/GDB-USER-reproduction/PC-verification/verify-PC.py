from pathlib import Path
import sys,json,hashlib,struct,zlib,csv
w=Path('/home/lolo/dev/msm/projects/MSM_vibe-TR2-001-autonome');sys.path.insert(0,str(w/'experiments/r3_durable_fat32/tools'));import read_campaign as reader
m=Path('/home/lolo/dev/msm/reports/TR2-001-autonome-reducteur-20261101/qualification-alimentation-20261102/GDB-USER-reproduction');p=m/'PC-copy/GI00005.TR2';o=m/'PC-verification';o.mkdir(exist_ok=True)
copy=json.loads((m/'PC-copy/copy.json').read_text(encoding='utf-8-sig'));sha=hashlib.file_digest(p.open('rb'),'sha256').hexdigest();assert sha==copy['SourceStreamSHA256']
r=reader.verify(p);assert r['status']=='INCOMPLETE';assert r['valid_xyz']==19035188 and r['valid_records']==244041;producer=(m/'observed-terminal-functions/source-sha.bin').read_bytes().hex();assert producer==r['sha256'];assert r['quality_or']==5
(o/'strict-reader.json').write_text(json.dumps(r,indent=2)+'\n');flagged=[]
with p.open('rb') as f:
 h=f.read(512)
 for seq in range(r['valid_records']):
  b=f.read(512);n,q=struct.unpack_from('<HH',b,36)
  if q&6:
   index=struct.unpack_from('<I',b,32)[0];t0,t1=struct.unpack_from('<QQ',b,16);samples=[struct.unpack_from('<hhh',b,44+6*j) for j in range(n)];candidates=[{'output_index':index+j,'source_index':10*(index+j),'xyz_counts':list(xyz),'interpolated_MCU_us':t0+(t1-t0)*j//max(n-1,1),'approx_time_since_first_output_s':(t0+(t1-t0)*j//max(n-1,1)-r['first_time_us'])/1e6} for j,xyz in enumerate(samples) if any(v in(-32768,32767)for v in xyz)]
   flagged.append({'record':seq,'quality_flags':q,'first_output_index':index,'count':n,'first_time_us':t0,'last_time_us':t1,'rail_value_candidates':candidates,'scope':'Per-block flags; exact incident raw native samples not archived'})
   (o/f'flagged-record-{seq}.bin').write_bytes(b)
 tail_start=f.tell();nonzero=False;remaining=0
 while chunk:=f.read(1048576):remaining+=len(chunk);nonzero|=any(chunk)
 assert not nonzero
 for record in flagged:
  seq=record['record'];start=max(0,seq-2);end=min(r['valid_records'],seq+3);f.seek((start+1)*512);b=f.read((end-start)*512);(o/f'context-{seq}.bin').write_bytes(b)
  with (o/f'context-{seq}.csv').open('w',newline='') as target:
   out=csv.writer(target);out.writerow(['output_index','record_quality_flags','interpolated_MCU_us','X_counts','Y_counts','Z_counts'])
   for off in range(0,len(b),512):
    block=b[off:off+512];n,q=struct.unpack_from('<HH',block,36);t0,t1=struct.unpack_from('<QQ',block,16);idx=struct.unpack_from('<I',block,32)[0]
    for j in range(n):out.writerow([idx+j,q,t0+(t1-t0)*j//max(n-1,1),*struct.unpack_from('<hhh',block,44+6*j)])
assert len(flagged)==1
result={'status':'INCOMPLETE_QUALITY_REJECTED','storage_payload_integrity':'ALL_XYZ_MATCH_INDEPENDENT_PRODUCER_RAM_SHA256','qualification_pass':False,'file_bytes':p.stat().st_size,'whole_file_SHA256':sha,'source_stream_SHA256':copy['SourceStreamSHA256'],'payload_SHA256':producer,'records':r['valid_records'],'XYZ':r['valid_xyz'],'payload_bytes':r['payload_bytes'],'all_record_CRC_sequence_session_padding_timestamps_verified':True,'all_remaining_preallocated_bytes_zero':True,'zero_tail_start':tail_start,'zero_tail_bytes':remaining,'no_COMPLETE_footer':True,'flagged_records':flagged,'remaining_limits':'Raw incident sensor samples unavailable; clipping does not establish mechanical/electrical origin. Duration nominal MCU, not SI. Strict importer unchanged.'}
(o/'GI0-PC-result.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
