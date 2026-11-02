from pathlib import Path
import json,hashlib,struct,zlib,sys
w=Path('/home/lolo/dev/msm/projects/MSM_vibe-TR2-001-GI0-publication');sys.path.insert(0,str(w/'experiments/r3_durable_fat32/tools'));import read_campaign as reader
p=Path('/home/lolo/dev/msm/reports/TR2-001-autonome-reducteur-20261101/qualification-alimentation-20261102/CN0-without-STLINK');file=p/'PC-copy/CN00005.TR2';out=p/'PC-verification';out.mkdir(exist_ok=False)
copy=json.loads((p/'PC-copy/copy.json').read_text(encoding='utf-8-sig'));whole=hashlib.file_digest(file.open('rb'),'sha256').hexdigest();assert whole==copy['SourceStreamSHA256'];r=reader.verify(file);ram=json.loads((p/'terminal-recovery/result.json').read_text());producer=(p/'terminal-recovery/producer-sha.bin').read_bytes().hex();assert r['status']=='COMPLETE' and r['sha256']==producer
assert r['valid_xyz']==ram['outputs']==19042924 and r['valid_records']*512==ram['sd_written']==125000192 and r['valid_records']==244141
assert r['actual_duration_us']==ram['duration_us']==7200000537 and r['quality_or']==1 and not ram['clips'] and not ram['rails']
assert r['firmware_sha']=='6633b0154bf67b4c5437f1d259732fc1b1f21e4a' and r['uid']==[0x004A003D,0x38425018,0x20333157] and r['cid']=='03534453443531328562e43cf80195eb'
assert r['logical_fs_g']==16 and r['sensitivity_ug_per_count']==488 and r['requested_duration_us']==7200000000
count=0;dig=hashlib.sha256();previous=None
with file.open('rb') as stream:
 h=stream.read(512);assert h==(p/'terminal-recovery/header.bin').read_bytes()
 for seq in range(r['valid_records']):
  b=stream.read(512);assert b[:4]==b'D3BK' and struct.unpack_from('<I',b,4)[0]==seq and struct.unpack_from('<Q',b,8)[0]==r['session'] and struct.unpack_from('<I',b,32)[0]==count
  assert struct.unpack_from('<I',b,40)[0]==zlib.crc32(b[:40]+bytes(4)+b[44:]);n,q=struct.unpack_from('<HH',b,36);assert 1<=n<=78 and (seq==r['valid_records']-1 or n==78) and q in(0,1);t0,t1=struct.unpack_from('<QQ',b,16);assert t1>=t0 and (previous is None or t0>previous);previous=t1;assert b[44+6*n:]==bytes(468-6*n)
  count+=n;dig.update(b[44:44+6*n])
 start=stream.tell();tail_remaining=file.stat().st_size-512-start;zeros=tail_remaining
 while tail_remaining:
  b=stream.read(min(tail_remaining,1048576));assert b and not any(b);tail_remaining-=len(b)
 footer=stream.read(512);assert footer[:8]==b'TR2END01' and zlib.crc32(footer[:508])==struct.unpack_from('<I',footer,508)[0];assert footer[96:128].hex()==producer and not stream.read(1)
assert count==ram['outputs'] and dig.hexdigest()==producer
(out/'header.bin').write_bytes(h);(out/'footer.bin').write_bytes(footer);(out/'strict-reader.json').write_text(json.dumps(r,indent=2)+'\n')
result={'status':'COMPLETE_PHYSICAL_STANDALONE_BOUNDED','file':'CN00005.TR2','source_SHA':r['firmware_sha'],'whole_file_bytes':file.stat().st_size,'whole_file_SHA256':whole,'source_stream_SHA256':copy['SourceStreamSHA256'],'XYZ':count,'record_count':r['valid_records'],'payload_bytes':count*6,'payload_SHA256':producer,'all_payload_equal_independent_RAM_producer':True,'independent_second_pass_CRC_sequence_session_timestamps_padding_counts':True,'zero_unused_bytes_before_footer':zeros,'actual_duration_nominal_MCU_us':ram['duration_us'],'clips':0,'rails':0,'scope':'2h nominal MCU, CN15 sector only, ST-LINK disconnected during capture, workshop setup; no SI duration, vibration calibration, reducer mounting or industrial robustness qualification'}
(out/'PC-result.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
