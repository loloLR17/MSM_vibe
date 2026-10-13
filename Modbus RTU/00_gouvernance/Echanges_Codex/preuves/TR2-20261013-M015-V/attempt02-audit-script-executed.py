from pathlib import Path
import json,struct,hashlib,statistics,zlib,datetime,sys,re
p=Path(sys.argv[1])
def binary(n): return (p/('coop4-'+n+'.bin')).read_bytes()
def scalar(n):return struct.unpack('<I',binary(n))[0]
def records(n,keys):
 b=binary(n);size=4*len(keys);assert len(b)%size==0
 return [dict(zip(keys,struct.unpack('<'+str(len(keys))+'I',b[i:i+size]))) for i in range(0,len(b),size)]
def cycle_ms(a,b):return ((b-a)&0xFFFFFFFF)/160000
words=binary('words');assert len(words)==36864*7
engine=binary('engine');assert len(engine)==140
wk='target_xyz received xyz timestamps polls bursts first_tick last_tick max_level last_level flags_or reason previous_counter counter_valid previous_ts ts_valid since_ts'.split()
window=dict(zip(wk,struct.unpack('<17I',engine[36:104])))
assert (window['xyz'],window['timestamps'],window['reason'],window['flags_or'])==(32768,4096,0,0)
assert struct.unpack('<I',engine[112:116])[0]==4 and engine[128:136]==bytes(8) and engine[136]==0x7b and engine[137:140]==bytes.fromhex('0c8000')
tr=records('traces','tick reg len write status error rx_left tx_left offset valid'.split())
pre=records('preclose','trace_index phase begin_cycles end_cycles tick requested state sr cr1 cr2 cfg1 cfg2 error_before_close rx_size rx_remaining tx_remaining rx_offset cs offset_valid'.split())
bursts=records('bursts','first_word words crc_payload begin_cycles end_cycles rx_before rx_after'.split())
modbus=records('modbus','tick state xyz address count rx_bytes tx_count tx_complete_tick'.split())
payload=binary('payload');by={i:[x for x in pre if x['trace_index']==i] for i in range(len(tr))}
transport_errors=[];statuses=[]
for i,t in enumerate(tr):
 assert t['offset']+t['len']<=len(payload)
 if not t['valid'] or t['status'] or t['error'] or t['rx_left'] or t['tx_left']:transport_errors.append(i)
 if not t['write']:
  rr=[x for x in by[i] if x['phase']==2]
  if len(rr)!=1:transport_errors.append(i);continue
  x=rr[0]
  if x['requested']!=t['len'] or x['rx_size']!=t['len'] or x['rx_remaining'] or x['rx_offset']!=t['len'] or not x['offset_valid'] or x['cs'] or x['error_before_close'] or x['sr']&64:transport_errors.append(i)
 if t['reg']==58 and not t['write']:
  b=payload[t['offset']:t['offset']+2];statuses.append({'trace':i,'tick':t['tick'],'level':int.from_bytes(b,'little')&1023,'flags':b[1]})
assert not transport_errors
assert all(s['level']<512 and not s['flags']&0x68 for s in statuses)
rebuild=b''.join(payload[t['offset']:t['offset']+t['len']] for t in tr if t['reg']==120 and not t['write'])
first=scalar('log-first');assert first==32256 and rebuild==words[first*7:] and len(rebuild)==4608*7
coverage=0;burst_lengths=[];duration=[];starts=[]
for b in bursts:
 assert b['first_word']==coverage and 0<b['words']<=128
 part=words[coverage*7:(coverage+b['words'])*7]
 assert zlib.crc32(part)==b['crc_payload']
 coverage+=b['words'];burst_lengths.append(b['words']);duration.append(cycle_ms(b['begin_cycles'],b['end_cycles']));starts.append(b['begin_cycles'])
assert coverage==36864 and len(bursts)==window['bursts']==304
cadence=[cycle_ms(a,b) for a,b in zip(starts,starts[1:])]
xyz=[];ts=[];errors=[];prev=None;since=0;prefix=[]
for off in range(0,len(words),7):
 b=words[off:off+7];tag=b[0]>>3;ctr=(b[0]>>1)&3
 if b[0].bit_count()%2:errors.append(['parity',off//7])
 if tag==2:
  if prev is not None and ctr!=(prev+1)%4:errors.append(['counter',off//7])
  prev=ctr;v=struct.unpack('<hhh',b[1:]);xyz.append(v);since+=1
  if any(x in (-32768,32767) for x in v):errors.append(['saturation',off//7])
  if len(xyz)%4096==0:prefix.append({'xyz':len(xyz),'timestamps':len(ts),'received_words':off//7+1,'last_counter':ctr,'last_timestamp':ts[-1] if ts else None})
 elif tag==4:
  v=int.from_bytes(b[1:5],'little')
  if ts and (since!=8 or not 0<(v-ts[-1])%2**32<2**31):errors.append(['timestamp',off//7])
  ts.append(v);since=0
 else:errors.append(['tag',off//7])
assert len(xyz)==32768 and len(ts)==4096 and not errors
prefix_xyz=list(struct.unpack('<8I',binary('prefix-xyz')));prefix_tick=list(struct.unpack('<8I',binary('prefix-tick')))
assert prefix_xyz==list(range(4096,32769,4096))
for i,x in enumerate(prefix):
 assert x['xyz']==prefix_xyz[i] and x['timestamps']==x['xyz']//8 and x['received_words']==x['xyz']*9//8
 x['tick']=prefix_tick[i]
 x['elapsed_ms']=prefix_tick[i]-scalar('start')
assert [(x['address'],x['count'],x['rx_bytes'],x['tx_count']) for x in modbus]==[(0,21,8,1),(1000,20,16,2)]
for x in modbus:
 assert x['state']==2 and scalar('start')<=x['tick']<x['tx_complete_tick']<=scalar('end')
 x['service_to_observed_TC_ms']=x['tx_complete_tick']-x['tick']
hpraw=(p/'hp-full-physical.jsonl').read_bytes();hp=[json.loads(x) for x in hpraw.decode('utf-8-sig').splitlines()]
assert hp[-1]['passed'] and hp[-1]['readsStarted']==2
lat=[]
for address in (0,1000):
 request=next(x for x in hp if x['event']=='read_intent' and x['start']==address)
 response=next(x for x in hp if x['event']=='decoded_response' and x['start']==address)
 lat.append({'address':address,'intent_to_decoded_response_ms':(datetime.datetime.fromisoformat(response['utc'])-datetime.datetime.fromisoformat(request['utc'])).total_seconds()*1000})
full=[i for i,t in enumerate(tr) if t['reg']==120 and t['len']==896]
pay_times=[cycle_ms(by[i][-1]['begin_cycles'],by[i][-1]['end_cycles']) for i in full]
worker=scalar('max-cycles')/160000
assert worker<896/210000*1000
final=(p/'final-state-no-reset.log').read_text(encoding='cp1252')
m=re.search('FINAL (.+)',final);assert m
native={k:int(v) for k,v in re.findall(r'(\w+)=(\d+)',m[1])}
assert native['crc_before']==native['crc_after']==zlib.crc32((p/'fram-before-capture.bin').read_bytes())
assert native['rx']==16 and native['tx']==2 and native['head']==native['tail']==20
assert native['stage']==2 and native['gen']==12 and native['coop']==4 and native['fifo']==3 and native['fram']==3 and native['core']==160000000
for n in ('result','errors','UART','dropped','failsafe','busy','fault','log_refused','preclose_overflow'): assert native[n]==0,n
assert 'CONFIGURATION_RECOVERY_UNAVAILABLE' in final
assert '0x00020000\t0x00000004' in final and '0xe000ed28:\t0x00000000\t0x00000000' in final
assert (p/'persistent-before.bin').read_bytes()==(p/'persistent-after-capture.bin').read_bytes()
rcc=binary('RCC');assert int.from_bytes(rcc[0xE8:0xEC],'little')&0x18==0x10
summary={'code_sha':'ef468539b9943cc9b9b4d832e513a0f8ab05c7ae','bounded_acceptance':True,'profile':'CPU160_HSI16_SPI4_MODE3','window':window,'xyz':len(xyz),'timestamps':len(ts),'distinct_xyz':len(set(xyz)),'parser_errors':errors,'timestamp_steps':sorted(set((b-a)%2**32 for a,b in zip(ts,ts[1:]))),'v1_prefixes':prefix,'bursts':len(bursts),'burst_words_min':min(burst_lengths),'burst_words_max':max(burst_lengths),'all_burst_crc32_match_RAM':True,'all_burst_byte_comparison_on_target':True,'last4096_full_SPI_buffer_equals_RAM':True,'full32768_SPI_raw_retained':False,'wire_external_capture':False,'full_words_sha256':hashlib.sha256(words).hexdigest(),'traces_last4096':len(tr),'preclose_last4096':len(pre),'transport_errors_last4096':transport_errors,'last4096_fifo_max':max(s['level'] for s in statuses),'last4096_fifo_last':statuses[-1]['level'],'start_tick':scalar('start'),'end_tick':scalar('end'),'whole_duration_ms':scalar('end')-scalar('start'),'worker_max_ms':worker,'nominal_128_word_budget_ms':896/210000*1000,'worker_budget_margin_ms':896/210000*1000-worker,'burst_work_before_CRC_mean_ms':statistics.mean(duration),'burst_work_before_CRC_max_ms':max(duration),'burst_cadence_mean_ms':statistics.mean(cadence),'burst_cadence_max_ms':max(cadence),'last4096_payload_mean_ms':statistics.mean(pay_times),'modbus':modbus,'hp_host_latencies':lat,'hp_full_journal_sha256':hashlib.sha256(hpraw).hexdigest(),'native_final':native,'fram_full_before_sha256':hashlib.sha256((p/'fram-before-capture.bin').read_bytes()).hexdigest(),'fram_after_evidence':'full CRC32 read, equal; active candidate byte equal; not full after SHA256','cleanup_hex':engine[137:140].hex(),'limits':['No endurance or global zero-loss proof','No independent clock/ODR metrology','No B5 coexistence qualification','No FFT, storage or vibratory V1 publication','Per-burst CRC and internal byte comparison for earlier windows; full SPI raw only final4096']}
for name,data in [('independent-audit.json',summary),('decoded-bursts.json',bursts),('decoded-modbus.json',modbus),('decoded-statuses-last4096.json',statuses),('decoded-traces-last4096.json',tr),('decoded-preclose-last4096.json',pre)]: (p/name).write_text(json.dumps(data,indent=2)+'\n')
remote=json.loads((p/'hp-cooperative4/stdout.log').read_text(encoding='utf-8-sig'))
assert remote['journalHash'].lower()==summary['hp_full_journal_sha256'] and remote['exitCode']==0
summary['hp_full_journal_remote_hash_verified']=True
(p/'independent-audit.json').write_text(json.dumps(summary,indent=2)+'\n')
(p/'audit-script-executed.py').write_bytes(Path(__file__).read_bytes())
print(json.dumps(summary,indent=2))
