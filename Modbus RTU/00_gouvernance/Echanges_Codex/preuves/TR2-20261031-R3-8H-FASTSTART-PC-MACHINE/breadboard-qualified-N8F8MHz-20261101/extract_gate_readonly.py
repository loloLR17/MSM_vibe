from pathlib import Path
import json,hashlib,importlib.util
import hardware_second as h
P=Path(__file__).parent;R=Path('/home/lolo/dev/msm/projects/MSM_vibe-R3-8h-faststart');d=P/'breadboard8-gate-collected';r=json.loads((d/'result.json').read_text());assert r['stage']==4 and r['error']==0 and r['header_written']==1 and r['run']==3
C=P/'file-chunks';C.mkdir(exist_ok=False);a=json.loads((P/'artifact.json').read_text());prefix=a['prefix'];body=h.IDENTITY+'''if r3_result.stage!=4 || r3_result.run!=3 || r3_result.error || fifo.running || r3_writer.active || r3_command || r3_clock_fault || q.count || r3_card.failed
 quit 8
end
set $writes_before=r3_card.writes
'''+f'set {{char[15]}} r3_header="0:/{prefix}0003.TR2"\n'+'''set $fr=(int)f_open(&'fat_volume.c'::file,(char*)r3_header,1)
p $fr
if $fr || 'fat_volume.c'::file.obj.objsize!=131072
 quit 9
end
'''
for i in range(16):
 body+='''set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
'''
 body+=f'dump binary memory {h.win(C/(str(i).zfill(2)+".bin")).replace(chr(92),"/")} r3_raw (r3_raw+8192)\n'
body+='''set $closed=(int)f_close(&'fat_volume.c'::file)
p $closed
p r3_card.writes
p r3_card.reads
p fifo.running
p r3_writer.active
p/x *(unsigned int*)0x42021414
if $closed || r3_card.writes!=$writes_before || r3_card.failed || fifo.running || r3_writer.active || !(*(unsigned int*)0x42021414 &0x1000)
 quit 11
end
'''
h.debug('gate-full-file-FatFs-readonly-copy',P/'arm/r3_coex.elf',body,timeout=60)
f=P/(prefix+'0003.TR2');assert not f.exists();f.write_bytes(b''.join((C/(str(i).zfill(2)+'.bin')).read_bytes() for i in range(16)))
spec=importlib.util.spec_from_file_location('reader',R/'experiments/r3_durable_fat32/tools/read_campaign.py');reader=importlib.util.module_from_spec(spec);spec.loader.exec_module(reader);v=reader.verify(f)
assert v['status']=='COMPLETE' and v['payload_bytes']==r['sd_total_bytes'] and v['valid_xyz']==r['outputs'] and v['actual_duration_us']==r['duration_us'] and v['firmware_sha']==a['code_sha'] and v['sha256']==(d/'source-sha.bin').read_bytes().hex()
v.update(file_sha256=hashlib.sha256(f.read_bytes()).hexdigest(),all_file_bytes_copied=True,read_path='FatFs on STM32 -> GDB memory dump -> host ordinary file I/O, not native PC card-reader extraction',upstream_RAM_digest_exact=True,no_card_write_during_copy=True,firmware_verifier_not_modified=True)
(P/'GATE_HOST_VERIFICATION.json').write_text(json.dumps(v,indent=2)+'\n');print(json.dumps(v,indent=2))
