from pathlib import Path
import json,struct,hashlib,statistics,csv
p=Path('/tmp/TR2-mission015-U/qualification')
raw=(p/'primary-result.bin').read_bytes();hdr=struct.unpack('<12I',raw[:48]);print('result header',hdr)
tk=['tick','reg','len','write','status','error','rx_left','tx_left','offset','valid'];pk=['trace_index','phase','begin_cycles','end_cycles','tick','requested','state','sr','cr1','cr2','cfg1','cfg2','error_before_close','rx_size','rx_remaining','tx_remaining','rx_offset','cs','offset_valid']
tb=(p/'primary-traces.bin').read_bytes();pb=(p/'primary-preclose.bin').read_bytes();payload=(p/'primary-payload.bin').read_bytes();words=(p/'primary-official-words.bin').read_bytes() if (p/'primary-official-words.bin').exists() else b''
tr=[dict(zip(tk,struct.unpack('<10I',tb[i:i+40]))) for i in range(0,len(tb),40)];pre=[dict(zip(pk,struct.unpack('<19I',pb[i:i+76]))) for i in range(0,len(pb),76)];by={i:[x for x in pre if x['trace_index']==i] for i in range(len(tr))}
rebuild=b''.join(payload[t['offset']:t['offset']+t['len']] for t in tr if t['reg']==120 and not t['write'])
xyz=[];ts=[];prev=None;since=0;errors=[];rows=[]
for i in range(0,len(words),7):
 b=words[i:i+7];tag=b[0]>>3;ctr=(b[0]>>1)&3
 if b[0].bit_count()%2:errors.append(['parity',i//7])
 row={'index':i//7,'hex':b.hex(),'tag':tag,'counter':ctr,'x':'','y':'','z':'','timestamp':''}
 if tag==2:
  if prev is not None and ctr!=(prev+1)%4:errors.append(['counter',i//7])
  prev=ctr;v=struct.unpack('<hhh',b[1:]);xyz.append(v);row.update(zip(['x','y','z'],v));since+=1
  if any(x in [-32768,32767] for x in v):errors.append(['saturation',i//7])
 elif tag==4:
  v=int.from_bytes(b[1:5],'little');row['timestamp']=v
  if ts and (since!=8 or not 0<(v-ts[-1])%2**32<2**31):errors.append(['timestamp',i//7])
  ts.append(v);since=0
 else:errors.append(['tag',i//7])
 rows.append(row)
statuses=[];transport=[]
for i,t in enumerate(tr):
 if not t['valid'] or t['status'] or t['error']:transport.append(i)
 if not t['write']:
  rr=[x for x in by[i] if x['phase']==2]
  if len(rr)!=1:transport.append(i);continue
  x=rr[0]
  if x['requested']!=t['len'] or x['rx_size']!=t['len'] or x['rx_remaining'] or x['rx_offset']!=t['len'] or not x['offset_valid'] or x['cs'] or x['error_before_close'] or x['sr']&64:transport.append(i)
 if t['reg']==58 and not t['write']:
  b=payload[t['offset']:t['offset']+2];statuses.append({'trace':i,'tick':t['tick'],'level':int.from_bytes(b,'little')&1023,'flags':b[1]})
fifo=[i for i,t in enumerate(tr) if t['reg']==120];full=[i for i in fifo if tr[i]['len']==896];intervals=[]
def dt(b,a):return ((b-a)&0xFFFFFFFF)/160000
for i,j in zip(fifo,fifo[1:]):
 if tr[i]['len']!=896:continue
 assert [tr[k]['reg'] for k in range(i,i+6)]==[120,58,7,15,25,58]
 n=tr[j]['len']//7
 idle=sum((int.from_bytes(payload[tr[k]['offset']:tr[k]['offset']+2],'little')&1023)<n for k in range(i+5,j) if tr[k]['reg']==58)
 span=dt(by[j][0]['begin_cycles'],by[i][0]['begin_cycles']);active=dt(by[i+5][-1]['end_cycles'],by[i][0]['begin_cycles'])
 intervals.append({'first':i,'next':j,'interval_ms':span,'delay_requests':idle,'busy_upper_ms':span-idle,'active_ms':active})
accepted=hdr[:6]==(6,0,0,4608,4096,512) and len(xyz)==4096 and len(ts)==512 and rebuild==words and not errors and not transport and all(not s['flags']&0x68 and s['level']<512 for s in statuses) and raw[4036:4039].hex()=='0c8000'
summary={'code_sha':'d18cf6a','header':hdr,'bounded_acceptance':accepted,'raw_words_sha256':hashlib.sha256(words).hexdigest(),'xyz':len(xyz),'timestamps':len(ts),'distinct_xyz':len(set(xyz)),'wire_buffer_byte_identical':rebuild==words,'parser_errors':errors,'transport_rejected':transport,'traces':len(tr),'payload_bytes':len(payload),'preclose':len(pre),'fifo_bursts':len(fifo),'max_level':max(s['level'] for s in statuses) if statuses else None,'last_level':statuses[-1]['level'] if statuses else None,'timestamp_steps':sorted(set((b-a)%2**32 for a,b in zip(ts,ts[1:]))),'cleanup_hex':raw[4036:4039].hex(),'nominal_budget_ms':896/210000*1000}
if intervals:summary.update(active_mean_ms=statistics.mean(x['active_ms'] for x in intervals),active_max_ms=max(x['active_ms'] for x in intervals),cadence_mean_ms=statistics.mean(x['interval_ms'] for x in intervals),busy_upper_max_ms=max(x['busy_upper_ms'] for x in intervals),delay_requests=sum(x['delay_requests'] for x in intervals),payload_mean_ms=statistics.mean(dt(by[i][-1]['end_cycles'],by[i][-1]['begin_cycles']) for i in full))
for name,data in [('independent-audit.json',summary),('statuses.json',statuses),('traces.json',tr),('preclose.json',pre)]: (p/name).write_text(json.dumps(data,indent=2)+'\n')
for name,data in [('words.csv',rows),('service.csv',intervals)]:
 if data:
  with (p/name).open('w') as f:w=csv.DictWriter(f,fieldnames=data[0].keys());w.writeheader();w.writerows(data)
print(json.dumps(summary,indent=2))
