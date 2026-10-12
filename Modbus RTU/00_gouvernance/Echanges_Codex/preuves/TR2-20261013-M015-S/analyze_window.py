from pathlib import Path
import struct,json,sys,importlib.util
P=Path(__file__).parent
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-M015-S')
name=sys.argv[1];i={'control_bin_sha256':json.loads((P/'artifacts.json').read_text())[name]['iis3dwb_reference.bin']['sha256']}
raw=(P/(name+'-result.bin')).read_bytes();assert len(raw)==4040
keys=['stage','result','cleanup_result','count','xyz_count','timestamp_count','who','raw_level','st_level','flags','status10','st_batch'];a=dict(zip(keys,struct.unpack('<12I',raw[:48])));a['controls_hex']=raw[48:62].hex();a['cleanup_controls_hex']=raw[4036:4039].hex()
tkeys=['tick','reg','len','write','status','error','rx_left','tx_left','offset','valid'];pkeys=['trace_index','phase','begin_cycles','end_cycles','tick','requested','state','sr','cr1','cr2','cfg1','cfg2','error_before_close','rx_size','rx_remaining','tx_remaining','rx_offset','cs','offset_valid']
tr=(P/(name+'-traces.bin')).read_bytes();ps=(P/(name+'-preclose.bin')).read_bytes();payload=(P/(name+'-payload.bin')).read_bytes();assert len(tr)%40==0 and len(ps)%76==0
traces=[dict(zip(tkeys,struct.unpack('<10I',tr[x:x+40]))) for x in range(0,len(tr),40)];pre=[dict(zip(pkeys,struct.unpack('<19I',ps[x:x+76]))) for x in range(0,len(ps),76)]
for x in pre:x['duration_us']=((x['end_cycles']-x['begin_cycles'])&0xFFFFFFFF)/16
words=b''.join(payload[t['offset']:t['offset']+t['len']] for t in traces if t['reg']==120 and not t['write'])
assert words==(P/(name+'-official-words.bin')).read_bytes()
statuses=[payload[t['offset']+1] for t in traces if t['reg']==58 and t['len']==2 and not t['write']]
config_ok=all(raw[48+x]==v for x,v in enumerate(bytes.fromhex('00010a867bac0044000000000000')) if x not in (6,13)) and int.from_bytes((P/(name+'-ctrl10.bin')).read_bytes(),'little')==32 and a['cleanup_controls_hex']=='0c8000'
spec=importlib.util.spec_from_file_location('qual',r/'experiments/iis3dwb_st_reference/tools/qualify_fifo.py');qual=importlib.util.module_from_spec(spec);spec.loader.exec_module(qual)
q=qual.analyze(words,expected_entries=a['count'],result=a['result'],cleanup=a['cleanup_result'],transfers=traces,statuses=statuses,config_ok=config_ok,trace_refused=int.from_bytes((P/(name+'-log-refused.bin')).read_bytes(),'little'))
rows=q.pop('rows');(P/(name+'-rows.json')).write_text(json.dumps(rows,indent=2)+'\n')
over=int.from_bytes((P/(name+'-overflow.bin')).read_bytes(),'little')
if over or a['stage']!=6 or a['xyz_count']!=q['xyz_count'] or a['timestamp_count']!=q['timestamp_count'] or q['xyz_distinct']<2 or q['saturated_components']:q['reasons'].append('extra_guard')
wk=['target_xyz','received','xyz','timestamps','polls','bursts','first_tick','last_tick','max_level','last_level','flags_or','reason','previous_counter','counter_valid','previous_ts','ts_valid','since_ts']
w=dict(zip(wk,struct.unpack('<17I',(P/(name+'-window.bin')).read_bytes())))
for ti,t in enumerate(traces):
 if t['write']:continue
 snapshots=[x for x in pre if x['trace_index']==ti and x['phase']==2]
 if len(snapshots)!=1:q['reasons'].append('missing_preclose');continue
 x=snapshots[0]
 if x['requested']!=t['len'] or x['rx_size']!=t['len'] or x['rx_remaining'] or not x['offset_valid'] or x['rx_offset']!=t['len'] or x['cs'] or x['error_before_close'] or x['sr']&0x40:q['reasons'].append('incomplete_preclose')
if q['xyz_count']!=4096 or w['received']!=q['entries'] or w['xyz']!=q['xyz_count'] or w['timestamps']!=q['timestamp_count'] or w['reason']:q['reasons'].append('window_mismatch')
q['window']=w;q['duration_ms']=((w['last_tick']-w['first_tick'])&0xFFFFFFFF) if a['result']==0 else None
q['captured_XYZ_per_second']=q['xyz_count']*1000/q['duration_ms'] if q['duration_ms'] else None
q['bounded_acceptance']=not q['reasons'];q['payload_words_identical']=True
q['transfer_us']=[x['duration_us'] for x in pre if x['phase']==2 and traces[x['trace_index']]['reg']==120]
q['measured_payload_bytes_per_second']=[x['requested']*1e6/x['duration_us'] for x in pre if x['phase']==2 and traces[x['trace_index']]['reg']==120 and x['duration_us']]
a.update(trace_count=len(traces),payload_bytes=len(payload),preclose_count=len(pre),overflow=over,reset_type='GDB monitor reset MCU; ST example sensor software reset after trigger; no manual power cycle',bin_sha256=i['control_bin_sha256'])
for suffix,data in [('analysis',a),('qualification',q),('trace-records',traces),('preclose-records',pre)]: (P/(name+'-'+suffix+'.json')).write_text(json.dumps(data,indent=2)+'\n')
print(json.dumps({'name':name,'result':a,'qualification':{k:v for k,v in q.items() if k not in ('timestamp_deltas','XYZ_between_timestamps','transfer_us','measured_payload_bytes_per_second')}},indent=2))
if not q['bounded_acceptance']:sys.exit(7)
