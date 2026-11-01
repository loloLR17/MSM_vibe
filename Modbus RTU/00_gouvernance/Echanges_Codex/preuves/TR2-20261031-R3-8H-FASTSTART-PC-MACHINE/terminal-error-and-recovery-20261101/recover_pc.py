from pathlib import Path
import subprocess,json,re,base64,hashlib,importlib.util,struct,datetime
P=Path(__file__).parent;D=P/'pc-recovery';D.mkdir(exist_ok=False);R=Path('/home/lolo/dev/msm/projects/MSM_vibe-R3-8h-faststart');PS='/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe'
def call(s):return subprocess.check_output([PS,'-NoProfile','-NonInteractive','-Command',"$ErrorActionPreference='Stop';"+s])
def win(p):return subprocess.check_output(['wslpath','-w',str(p)],text=True).strip()
x=call("@(Get-Disk | Where-Object {$_.FriendlyName -eq 'Realtek PCIE CardReader' -and $_.Size -eq 31914983424} | Get-Partition | Where-Object DriveLetter | Select DiskNumber,PartitionNumber,DriveLetter) | ConvertTo-Json -Compress");(D/'discovery.json').write_bytes(x);v=json.loads(x.decode('utf-8-sig'));v=v if isinstance(v,list) else [v];assert len(v)==1;drive=v[0]['DriveLetter'];assert re.fullmatch('[D-Z]',drive)
for n,s in [('volume.json',f'Get-Volume -DriveLetter {drive} | Select DriveLetter,FileSystem,Size,SizeRemaining,AllocationUnitSize,HealthStatus | ConvertTo-Json'),('physical-disk.json',f'Get-Partition -DriveLetter {drive} | Get-Disk | Select Number,FriendlyName,SerialNumber,BusType,PartitionStyle,Size | ConvertTo-Json'),('files.json',f'Get-ChildItem -LiteralPath {drive}:\\ | Select Name,Length | ConvertTo-Json')]: (D/n).write_bytes(call(s))
assert json.loads((D/'volume.json').read_text(encoding='utf-8-sig'))['FileSystem']=='FAT32'
info=base64.b64decode(call(f"[Convert]::ToBase64String([IO.File]::ReadAllBytes('{drive}:\\TR2INFO.TXT'))").strip());assert info==Path('/home/lolo/dev/msm/reports/TR2-r3-1h-fat32-endurance/pc-before/TR2INFO.TXT').read_bytes();(D/'TR2INFO.TXT').write_bytes(info)
spec=importlib.util.spec_from_file_location('reader',R/'experiments/r3_durable_fat32/tools/read_campaign.py');reader=importlib.util.module_from_spec(spec);spec.loader.exec_module(reader)
results=[]
for name,size in [('N8G0003.TR2',131072),('N8F0003.TR2',131072),('N8E0003.TR2',131072),('N8G0005.TR2',671088640)]:
 out=D/name;call("[IO.File]::Copy('"+drive+":\\"+name+"', '"+win(out)+"', $false)")
 expected=call("(Get-FileHash -Algorithm SHA256 -LiteralPath '"+drive+":\\"+name+"').Hash").decode().strip().lower();digest=hashlib.file_digest(out.open('rb'),'sha256').hexdigest();assert out.stat().st_size==size and digest==expected
 with out.open('rb') as f:header=f.read(512)
 assert list(struct.unpack_from('<3I',header,24))==[0x001b0046,0x38425018,0x20333157] and header[36:52].hex()=='0353445344333247858bf0923c01927b'
 parsed=reader.verify(out)
 if name=='N8F0003.TR2':assert digest=='e8227c336ef2e488b0dc12da807402dd1209f8268c44a0fe4ef1def6bfa54e6d'
 if name=='N8G0003.TR2':assert parsed['sha256']==(P/'n8g-gate-collected/source-sha.bin').read_bytes().hex() and parsed['status']=='COMPLETE'
 if name=='N8E0003.TR2':assert parsed['sha256']==Path('/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/breadboard-gate-collected/source-sha.bin').read_bytes().hex() and parsed['status']=='COMPLETE'
 row=dict(name=name,whole_file_sha256=digest,card_equals_copy=True,parsed=parsed);(D/(name+'.json')).write_text(json.dumps(row,indent=2)+'\n');results.append(row);print(name,parsed['status'],parsed['valid_xyz'],parsed['payload_bytes'],flush=True)
long=results[-1]['parsed'];ram=json.loads((P/'long-error-terminal/result.json').read_text());suffix=(P/'long-error-terminal/volatile-suffix-XYZ.bin').read_bytes();assert long['status']=='INCOMPLETE' and long['valid_records']*512==ram['sd_written'] and long['valid_xyz']+len(suffix)//6==ram['outputs']
recovered=D/'RECOVERED_PRODUCER_XYZ_INT16.bin';digest=hashlib.sha256()
with (D/'N8G0005.TR2').open('rb') as source,recovered.open('xb') as out:
 source.seek(512)
 for _ in range(long['valid_records']):
  block=source.read(512);n=struct.unpack_from('<H',block,36)[0];data=block[44:44+6*n];out.write(data);digest.update(data)
 out.write(suffix);digest.update(suffix)
assert recovered.stat().st_size==ram['sd_total_bytes'] and digest.digest()==(P/'long-error-terminal/source-sha.bin').read_bytes()
result=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),real_native_PC_reader=True,read_only_on_card=True,TR2INFO_exact=True,short_files=results[:-1],original_long=results[-1],recovered_producer_payload_file=recovered.name,recovered_payload_bytes=recovered.stat().st_size,recovered_payload_XYZ=ram['outputs'],recovered_payload_sha256=digest.hexdigest(),all_produced_filtered_XYZ_exact_to_independent_RAM_digest=True,source_of_tail_XYZ=262,tail_source='volatile queue and unflushed frame saved before poweroff',strict_original_status='INCOMPLETE',eight_hours_qualified=False,nominal_MCU_capture_duration_us=ram['duration_us'],no_fabricated_footer_or_original_modification=True)
(D/'RECOVERY.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({k:v for k,v in result.items() if k not in ['short_files','original_long']},indent=2))
