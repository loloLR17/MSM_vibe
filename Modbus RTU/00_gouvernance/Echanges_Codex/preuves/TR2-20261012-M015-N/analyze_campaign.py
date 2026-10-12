from pathlib import Path
import json,sys,csv,hashlib
P=Path(__file__).parent
sys.path.insert(0,str(Path('experiments/iis3dwb_st_reference/tools').resolve()))
from qualify_fifo import analyze
summaries=[]
for name in ('reference','no-ts','bdr-last','word7'):
 a=json.loads((P/(name+'-analysis.json')).read_text());raw=(P/(name+'-result.bin')).read_bytes();tr=json.loads((P/(name+'-trace-records.json')).read_text());ps=json.loads((P/(name+'-preclose-records.json')).read_text());payload=(P/(name+'-payload.bin')).read_bytes();words=raw[62:62+a['count']*7]
 fifo=[dict(index=i,**t) for i,t in enumerate(tr) if t['reg']==120 and not t['write']]
 assert b''.join(payload[t['offset']:t['offset']+t['len']] for t in fifo)==words
 statuses=[payload[t['offset']+1] for t in tr if t['reg']==58 and t['len']==2]
 levels=[(payload[t['offset']]|(payload[t['offset']+1]<<8))&1023 for t in tr if t['reg']==58 and t['len']==2]
 q=analyze(words,expected_entries=8 if name=='word7' else 512,result=a['result'],cleanup=a['cleanup_result'],transfers=tr,statuses=statuses,trace_refused=a['log_refused'],timestamp_decimation=0 if name=='no-ts' else 8)
 for s in ps:
  if s['phase']==2:assert s['rx_remaining']==0 and s['rx_offset']==s['requested'] and s['offset_valid'] and not s['sr']&64
 rows=q.pop('rows');ts=[x['timestamp'] for x in rows if x['tag']==4]
 q.update(level_count=len(levels),level_min=min(levels),level_max=max(levels),payload_words_identical=True,source_sha=(P/'code-sha').read_text(),timestamp_first=ts[0] if ts else None,timestamp_last=ts[-1] if ts else None,timestamp_delta_values=sorted(set(q['timestamp_deltas'])))
 (P/(name+'-qualification.json')).write_text(json.dumps(q,indent=2)+'\n');(P/(name+'-words.bin')).write_bytes(words)
 if rows:
  with (P/(name+'-words.csv')).open('w',newline='') as f:w=csv.DictWriter(f,fieldnames=sorted(set().union(*(x.keys() for x in rows))),lineterminator='\n');w.writeheader();w.writerows(rows)
 times=[s['duration_us'] for s in ps if s['phase']==2 and any(t['index']==s['trace_index'] for t in fifo)]
 summary=dict(profile=name,firmware_result=a['result'],cleanup=a['cleanup_result'],entries=a['count'],xyz=q['xyz_count'],timestamps=q['timestamp_count'],parity_errors=q['parity_errors'],counter_errors=q['counter_discontinuities'],traces=a['trace_count'],polls=a['polls'],refused=a['log_refused'],overflow=a['overflow'],level_min=q['level_min'],level_max=q['level_max'],timestamp_first=q['timestamp_first'],timestamp_last=q['timestamp_last'],timestamp_steps=q['timestamp_delta_values'],fifo_read_us=times,config_hex=a['controls_hex'],cleanup_controls=a['cleanup_controls_hex'],qualification=q['reasons'])
 summaries.append(summary);print(json.dumps(summary))
(P/'campaign-summary.json').write_text(json.dumps(summaries,indent=2)+'\n')
