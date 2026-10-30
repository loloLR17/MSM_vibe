from pathlib import Path
import json,zlib,subprocess,hashlib,importlib.util,datetime,sys
P=Path(__file__).parent;R=Path('/home/lolo/dev/msm/projects/MSM_vibe-R3-1h-E4');sha=subprocess.check_output(['git','rev-parse','origin/main'],cwd=R,text=True).strip();proof='Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261029-R3-1H-FAT32-ENDURANCE/E4-pc'
def get(path):return subprocess.check_output(['git','show',sha+':'+path],cwd=R)
manifest=json.loads(get(proof+'/BUNDLE.json'));by_source={r['source']:r for r in manifest}
def raw(source):
 r=by_source[source];packed=get(proof+'/'+r['file']);b=zlib.decompress(packed);assert hashlib.sha256(packed).hexdigest()==r['stored_sha256'] and hashlib.sha256(b).hexdigest()==r['sha256'];return b
spec=importlib.util.spec_from_file_location('reader',R/'experiments/r3_durable_fat32/tools/read_campaign.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
assert get('experiments/r3_durable_fat32/tools/read_campaign.py')==(R/'experiments/r3_durable_fat32/tools/read_campaign.py').read_bytes()
extents=json.loads(raw('pc-extents/EXTENTS.json'));out=P/('E4-remote-reconstruction-'+sys.argv[1]);out.mkdir(exist_ok=False);rows=[]
for row in extents:
 f=out/row['name'];prefix=raw('pc-extents/'+row['header_records_file']);footer=raw('pc-extents/'+row['footer_file'])
 with f.open('xb') as stream:stream.write(prefix);stream.seek(row['footer_offset']);stream.write(footer)
 v=m.verify(f);expected=json.loads(raw('pc-final/'+row['name']+'.json'));assert v['sha256']==expected['sha256']==row['payload_sha256'] and v['valid_records']==expected['valid_records'] and v['valid_xyz']==expected['valid_xyz'] and v['status']=='COMPLETE'
 assert hashlib.sha256(f.read_bytes()).hexdigest()==row['reconstructed_sha256'] and row['reconstructed_sha256']!=row['original_PC_whole_sha256']
 rows.append(dict(name=row['name'],all_committed_records_CRC_indices_footer_verified=True,payload_bytes=v['payload_bytes'],payload_sha256=v['sha256'],valid_xyz=v['valid_xyz'],source_firmware_sha=v['firmware_sha'],unused_original_tail_not_in_GitHub=True,reconstructed_is_not_original_PC_file=True))
result=dict(sha=sha,utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),source='Fetched GitHub objects, actual compressed extents reconstructed and parsed',all_pass=True,files=rows);(P/('remote-E4-extents-'+sys.argv[1]+'.json')).write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
