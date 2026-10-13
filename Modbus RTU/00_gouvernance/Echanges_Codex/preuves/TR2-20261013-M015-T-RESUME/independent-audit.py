from pathlib import Path
import json,struct,statistics,csv,hashlib,subprocess
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-M015-T-resume');p=Path(Path('/tmp/tr2-m015-resume-proof').read_text());summaries=[]
for name in ['cpu160spi4','cpu160spi8']:
 raw=(p/(name+'-result.bin')).read_bytes();assert struct.unpack('<6I',raw[:24])==(6,0,0,4608,4096,512) and raw[4036:4039].hex()=='0c8000'
 tb=(p/(name+'-traces.bin')).read_bytes();pb=(p/(name+'-preclose.bin')).read_bytes();payload=(p/(name+'-payload.bin')).read_bytes();words=(p/(name+'-official-words.bin')).read_bytes()
 tk=['tick','reg','len','write','status','error','rx_left','tx_left','offset','valid'];pk=['trace_index','phase','begin_cycles','end_cycles','tick','requested','state','sr','cr1','cr2','cfg1','cfg2','error_before_close','rx_size','rx_remaining','tx_remaining','rx_offset','cs','offset_valid']
 tr=[dict(zip(tk,struct.unpack('<10I',tb[i:i+40]))) for i in range(0,len(tb),40)];pre=[dict(zip(pk,struct.unpack('<19I',pb[i:i+76]))) for i in range(0,len(pb),76)];by={i:[x for x in pre if x['trace_index']==i] for i in range(len(tr))}
 assert all(t['valid'] and not t['status'] and not t['error'] for t in tr)
 assert b''.join(payload[t['offset']:t['offset']+t['len']] for t in tr if t['reg']==120 and not t['write'])==words
 xyz=[];ts=[];since=0;prev=None;rows=[]
 for i in range(0,len(words),7):
  b=words[i:i+7];tag=b[0]>>3;ctr=(b[0]>>1)&3;assert b[0].bit_count()%2==0
  row={'index':i//7,'raw_hex':b.hex(),'tag':tag,'counter':ctr,'x':'','y':'','z':'','TS':''}
  if tag==2:
   if prev is not None:assert ctr==(prev+1)%4
   prev=ctr;v=struct.unpack('<hhh',b[1:]);xyz.append(v);row.update(zip(['x','y','z'],v));since+=1
  elif tag==4:
   v=int.from_bytes(b[1:5],'little');row['TS']=v
   if ts:assert since==8 and 0<(v-ts[-1])%2**32<2**31
   ts.append(v);since=0
  else:raise AssertionError('unknown tag')
  rows.append(row)
 assert len(xyz)==4096 and len(ts)==512 and not any(v in(-32768,32767) for t in xyz for v in t)
 statuses=[]
 for i,t in enumerate(tr):
  if not t['write']:
   rr=[x for x in by[i] if x['phase']==2];assert len(rr)==1;x=rr[0]
   assert x['requested']==t['len'] and x['rx_size']==t['len'] and not x['rx_remaining'] and x['rx_offset']==t['len'] and x['offset_valid'] and not x['cs'] and not x['error_before_close'] and not x['sr']&64
  if t['reg']==58 and not t['write']:
   b=payload[t['offset']:t['offset']+2];assert not b[1]&0x68;statuses.append({'trace':i,'tick':t['tick'],'level':int.from_bytes(b,'little')&1023,'flags':b[1]})
 fifo=[i for i,t in enumerate(tr) if t['reg']==120];full=[i for i in fifo if tr[i]['len']==896];intervals=[]
 def dt(b,a):return ((b-a)&0xFFFFFFFF)/160000
 for i,j in zip(fifo,fifo[1:]):
  if tr[i]['len']!=896:continue
  assert [tr[k]['reg'] for k in range(i,i+6)]==[120,58,7,15,25,58]
  n=tr[j]['len']//7
  idle=sum((int.from_bytes(payload[tr[k]['offset']:tr[k]['offset']+2],'little')&1023)<n for k in range(i+5,j) if tr[k]['reg']==58)
  span=dt(by[j][0]['begin_cycles'],by[i][0]['begin_cycles']);active=dt(by[i+5][-1]['end_cycles'],by[i][0]['begin_cycles'])
  intervals.append({'first':i,'next':j,'interval_ms':span,'actual_delay_requests':idle,'busy_upper_ms':span-idle,'active_to_first_pre_status_end_ms':active})
 clkraw=(p/(name+'-RCC.bin')).read_bytes();pwr=(p/(name+'-PWR.bin')).read_bytes();flash=(p/(name+'-FLASH-ACR.bin')).read_bytes();get=lambda data,off:struct.unpack_from('<I',data,off)[0]
 assert get(clkraw,0)&0x400 and get(clkraw,0xE8)&0x18==0x10 and get(clkraw,0x24)&0x70==0 and get(pwr,0xC)&0x38000==0x38000 and get(flash,0)&15==4
 div=4 if name.endswith('4') else 2;expected=1 if div==4 else 0
 assert all((x['cfg1']>>28)&7==expected for x in pre)
 q=json.loads((p/(name+'-qualification.json')).read_text());assert q['bounded_acceptance']
 s={'capture':name,'CPU_Hz':160000000,'SPI_source':'HSI16','calculated_SPI_Hz':16000000//div,'physical_SPI_measured':False,'XYZ':len(xyz),'TS':len(ts),'distinct':len(set(xyz)),'reconstruction':'BYTE_IDENTICAL','raw_sha256':hashlib.sha256(words).hexdigest(),'all_guards_passed':True,'parity_errors':0,'counter_discontinuities':0,'FIFO_flags_OR':0,'max_FIFO_level':max(x['level'] for x in statuses),'final_FIFO_level':statuses[-1]['level'],'duration_ms':q['duration_ms'],'trace_count':len(tr),'payload_log_bytes':len(payload),'snapshots':len(pre),'payload_full_mean_ms':statistics.mean(dt(by[i][-1]['end_cycles'],by[i][-1]['begin_cycles']) for i in full),'active_service_mean_ms':statistics.mean(x['active_to_first_pre_status_end_ms'] for x in intervals),'active_service_max_ms':max(x['active_to_first_pre_status_end_ms'] for x in intervals),'cadence_with_wait_mean_ms':statistics.mean(x['interval_ms'] for x in intervals),'requested_idle_count':sum(x['actual_delay_requests'] for x in intervals),'busy_upper_mean_ms':statistics.mean(x['busy_upper_ms'] for x in intervals),'busy_upper_max_ms':max(x['busy_upper_ms'] for x in intervals),'nominal_budget_ms':896/210000*1000,'cleanup':'0c8000','global_loss_zero_proven':False,'timestamp_delta_histogram':dict(__import__('collections').Counter((b-a)%2**32 for a,b in zip(ts,ts[1:])))}
 s['active_service_time_margin_ms']=s['nominal_budget_ms']-s['active_service_max_ms'];s['conservative_busy_upper_margin_ms']=s['nominal_budget_ms']-s['busy_upper_max_ms'];summaries.append(s)
 for suffix,data in [('audit',s),('statuses',statuses)]: (p/(name+'-independent-'+suffix+'.json')).write_text(json.dumps(data,indent=2)+'\n')
 for suffix,data in [('words',rows),('service',intervals)]:
  with (p/(name+'-'+suffix+'.csv')).open('w',newline='') as out:w=csv.DictWriter(out,fieldnames=list(data[0]),lineterminator='\n');w.writeheader();w.writerows(data)
(p/'final-comparison.json').write_text(json.dumps(summaries,indent=2)+'\n');(p/'independent-audit.py').write_bytes(Path(__file__).read_bytes());print(json.dumps(summaries,indent=2))
