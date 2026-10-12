from pathlib import Path
import json,struct
P=Path(__file__).parent
name="control"
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
a['transport_method_compiled']=0
a['metadata_symbol_absent_due_linker_gc']=True
a['ctrl10_before_cleanup']=int.from_bytes((P/(name+'-ctrl10.bin')).read_bytes(),'little')
a['transport_success']=a['stage']==6 and a['result']==0 and a['cleanup_result']==0 and not failed and not a['overflow'] and not a['log_refused']
(P/(name+'-analysis.json')).write_text(json.dumps(a,indent=2)+'\n');(P/(name+'-trace-records.json')).write_text(json.dumps(traces,indent=2)+'\n');(P/(name+'-preclose-records.json')).write_text(json.dumps(pre,indent=2)+'\n');print(json.dumps(a,indent=2))
