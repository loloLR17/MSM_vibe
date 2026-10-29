from pathlib import Path
import json,subprocess,hashlib,zlib,datetime,struct,sys
ROOT=Path('/home/lolo/dev/msm/projects/MSM_vibe-accel-fs8g-ac-resume');P=Path(__file__).parent
sha=subprocess.check_output(['git','rev-parse','origin/main'],text=True,cwd=ROOT).strip()
E='Modbus RTU/00_gouvernance/Echanges_Codex';proof=E+'/preuves/TR2-20261029-R3-COEX-FIFO-FIR-SPI2'
paths=subprocess.check_output(['git','ls-tree','-r','--name-only',sha,'--',proof,'experiments/r3_coex_fifo_fir_sd',E+'/DERNIER_RAPPORT.md',E+'/MISSION_EN_COURS.md','Modbus RTU/00_gouvernance/ETAT_COURANT_TR2.md',E+'/archives/TR2-20261029-R3-COEX-FIFO-FIR-SPI2'],text=True,cwd=ROOT).splitlines()
records=[];data={}
for f in paths:
 b=subprocess.check_output(['git','show',sha+':'+f],cwd=ROOT);assert b==(ROOT/f).read_bytes(),f;data[f]=b;records.append(dict(path=f,sha256=hashlib.sha256(b).hexdigest()))
artifacts=0;final={}
for f in paths:
 if f.endswith('/BUNDLE.json'):
  rows=json.loads(data[f]);assert isinstance(rows,list)
  for row in rows:
   b=data[str(Path(f).parent/row['file'])];raw=zlib.decompress(b)
   assert len(raw)==row['bytes'] and hashlib.sha256(raw).hexdigest()==row['sha256'] and hashlib.sha256(b).hexdigest()==row['stored_sha256'];artifacts+=1
   if '/final-queue64/' in f:final[row['source']]=raw
assert artifacts>=616
media=[]
for label in ['c3-2s-queue64','c3-10s-queue64','c4-60s-queue64']:
 it=json.loads(final[label+'-integrity.json']);full=final[label+'-media-full.bin'];header=final[label+'-media-header.bin'];upstream=final[label+'/source-sha.bin'];size=it['payload_bytes']
 assert hashlib.sha256(full[:size]).digest()==upstream==header[64:96]
 assert full[size:]==b'\xff'*(len(full)-size) and zlib.crc32(header[:508])==struct.unpack_from('<I',header,508)[0]
 outputs=list(struct.iter_unpack('<QQQQhhhBBB7x',final[label+'/outputs.bin']));prefix=b''.join(struct.pack('<hhh',*o[4:7]) for o in outputs);assert full[:len(prefix)]==prefix
 media.append(dict(case=label,payload_bytes=size,sha256=upstream.hex(),persisted_prefix=len(outputs),all_equal=True))
result=dict(sha=sha,utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),source='GitHub origin/main freshly fetched; complete git objects re-read, compared byte-for-byte',all_equal=True,files_count=len(paths),decompressed_artifacts=artifacts,media=media,files=records)
out=P/('remote-queue64-'+sys.argv[1]+'.json');assert not out.exists();out.write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({k:v for k,v in result.items() if k!='files'},indent=2))
