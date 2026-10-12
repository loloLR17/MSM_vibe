from pathlib import Path
import sys,subprocess,json,hashlib,time,struct
import hardware_second as h
P=Path(__file__).parent
name=sys.argv[1];assert name in ('reference','no-ts','bdr-last','word7')
h.guard()
if name!='reference':
    ref=json.loads((P/'reference-analysis.json').read_text())
    assert ref['known_timestamp_only'],ref
    prior={'no-ts':'reference','bdr-last':'no-ts','word7':'bdr-last'}[name]
    if prior!='reference':
        prev=json.loads((P/(prior+'-analysis.json')).read_text())
        assert prev['allow_next_diagnostic'],prev
b=P/name;image=b/'iis3dwb_reference.bin';elf=b/'iis3dwb_reference.elf'
manifest=json.loads((P/'artifacts.json').read_text())[name]
for f in (image,elf):assert hashlib.sha256(f.read_bytes()).hexdigest()==manifest[f.name]['sha256']
identity='x/3wx 0x0BFA0700\nx/wx 0xE0044000\nif *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157\n quit 3\nend\n'
h.debug(name+'-identity-before-flash',identity)
command=[h.CLI,'-c','port=SWD','sn='+h.SN,'freq=8000','-w',h.win(image),'0x08000000','-v','-rst']
with (P/(name+'-flash-command.json')).open('x') as f:json.dump({'command':command,'source_sha':(P/'code-sha').read_text(),'bin_sha256':manifest[image.name]['sha256'],'elf_sha256':manifest[elf.name]['sha256'],'sn':h.SN,'uid':'001B00463842501820333157','no_full_flash_backup_or_restore':True},f,indent=2)
r=subprocess.run(command,capture_output=True,timeout=45);(P/(name+'-flash.log')).write_bytes(r.stdout+r.stderr);r.check_returncode();assert b'Download verified successfully' in r.stdout
prefix=h.win(P).replace('\\','/')
h.debug(name+'-readback-reset',identity+f'dump binary memory {prefix}/{name}-flash.bin 0x08000000 {hex(0x08000000+image.stat().st_size)}\nmonitor reset\n',elf)
assert (P/(name+'-flash.bin')).read_bytes()==image.read_bytes()
time.sleep(.2)
h.debug(name+'-single-trigger','p bench_ready\np bench_uid\np bench_core_hz\np bench_result\nx/2wx 0xE000ED28\nif bench_ready != 1 || bench_core_hz != 16000000 || bench_uid[0] != 0x001B0046 || bench_uid[1] != 0x38425018 || bench_uid[2] != 0x20333157\n quit 4\nend\nif *(unsigned int *)0xE000ED28 != 0 || *(unsigned int *)0xE000ED2C != 0\n quit 5\nend\nset var bench_command=0x4D31354B\n',elf)
time.sleep(1)
body='p bench_result\np bench_trace_count\np bench_payload_count\np bench_preclose_count\np bench_preclose_overflow\np sizeof(BenchResult)\np sizeof(BenchTrace)\np sizeof(BenchPreclose)\np bench_uid\np bench_core_hz\nx/2wx 0xE000ED28\nx/6wx 0x42020800\n'
for label,start,end in [('result','&bench_result','(&bench_result+1)'),('traces','bench_trace','((char *)bench_trace)+bench_trace_count*sizeof(BenchTrace)'),('payload','bench_payload','bench_payload+bench_payload_count'),('preclose','bench_preclose','((char *)bench_preclose)+bench_preclose_count*sizeof(BenchPreclose)'),('overflow','&bench_preclose_overflow','(&bench_preclose_overflow+1)'),('log-refused','&bench_log_refused','(&bench_log_refused+1)'),('poll-aggregate','bench_poll_aggregate','bench_poll_aggregate+6')]:
 body+=f'dump binary memory {prefix}/{name}-{label}.bin {start} {end}\n'
h.debug(name+'-capture',body,elf)
raw=(P/(name+'-result.bin')).read_bytes();assert len(raw)==4040
keys=['stage','result','cleanup_result','count','xyz_count','timestamp_count','who','raw_level','st_level','flags','status10','st_batch'];a=dict(zip(keys,struct.unpack('<12I',raw[:48])));a['controls_hex']=raw[48:62].hex()
tr=(P/(name+'-traces.bin')).read_bytes();ps=(P/(name+'-preclose.bin')).read_bytes();assert len(tr)%40==0 and len(ps)%76==0
tkeys=['tick','reg','len','write','status','error','rx_left','tx_left','offset','valid'];pkeys=['trace_index','phase','begin_cycles','end_cycles','tick','requested','state','sr','cr1','cr2','cfg1','cfg2','error_before_close','rx_size','rx_remaining','tx_remaining','rx_offset','cs','offset_valid']
traces=[dict(zip(tkeys,struct.unpack('<10I',tr[i:i+40]))) for i in range(0,len(tr),40)];pre=[dict(zip(pkeys,struct.unpack('<19I',ps[i:i+76]))) for i in range(0,len(ps),76)]
for t in pre:t['duration_cycles']=(t['end_cycles']-t['begin_cycles'])&0xFFFFFFFF;t['duration_us']=t['duration_cycles']/16
failed=[dict(trace_index=i,**t) for i,t in enumerate(traces) if not t['valid']]
bursts=[dict(trace_index=i,**t) for i,t in enumerate(traces) if t['reg']==0x78 and t['len']==896 and not t['write']]
a.update(trace_count=len(traces),preclose_count=len(pre),overflow=struct.unpack('<I',(P/(name+'-overflow.bin')).read_bytes())[0],failed=failed,bursts=bursts,burst_preclose=[t for t in pre if t['phase']==2 and t['requested']==896])
a['log_refused']=int.from_bytes((P/(name+'-log-refused.bin')).read_bytes(),'little')
a['polls']=int.from_bytes(raw[4032:4036],'little');a['cleanup_controls_hex']=raw[4036:4039].hex()
a['aggregate']=struct.unpack('<6I',(P/(name+'-poll-aggregate.bin')).read_bytes())
a['known_timestamp_only']=name=='reference' and a['stage']==6 and a['result']==0 and a['cleanup_result']==0 and a['count']==512 and a['xyz_count']==0 and a['timestamp_count']==512 and not failed and not a['overflow'] and not a['log_refused']
a['allow_next_diagnostic']=a['cleanup_result']==0 and not failed and not a['overflow'] and not a['log_refused'] and a['xyz_count']==0 and (a['result']==0 or (name=='no-ts' and a['result'] in (4,10) and a['count']==0 and a['raw_level']==0))
a['transport_success']=a['stage']==6 and a['result']==0 and a['cleanup_result']==0 and a['count']==(8 if name=='word7' else 512) and not failed and not a['overflow'] and not a['log_refused']
(P/(name+'-analysis.json')).write_text(json.dumps(a,indent=2)+'\n');(P/(name+'-trace-records.json')).write_text(json.dumps(traces,indent=2)+'\n');(P/(name+'-preclose-records.json')).write_text(json.dumps(pre,indent=2)+'\n');print(json.dumps(a,indent=2))
