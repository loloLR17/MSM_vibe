from pathlib import Path
import json,struct,zlib,hashlib,statistics
p=Path(Path('/tmp/tr2-a0402-proof').read_text())
def raw(n):return (p/('physical-'+n+'.bin')).read_bytes()
def scalar(n):return struct.unpack('<I',raw(n))[0]
def records(n,keys):
 data=raw(n);width=4*len(keys);assert len(data)%width==0;return [dict(zip(keys,struct.unpack('<'+str(len(keys))+'I',data[i:i+width]))) for i in range(0,len(data),width)]
engine=raw('engine');assert len(engine)==140
keys='target_xyz received xyz timestamps polls bursts first_tick last_tick max_level last_level flags_or reason previous_counter counter_valid previous_ts ts_valid since_ts'.split()
window=dict(zip(keys,struct.unpack('<17I',engine[36:104])))
assert [window[k] for k in ('target_xyz','received','xyz','timestamps','reason')]==[32768,36864,32768,4096,0]
assert not window['flags_or'] & 0x68  # Existing FULL/OVR/LATCHED guard; WM and level bits are valid.
assert struct.unpack('<I',engine[112:116])[0]==4 and engine[128:136]==bytes(8) and engine[136]==123 and engine[137:140]==bytes.fromhex('0c8000')
words=raw('words');assert len(words)==258048
bursts=records('bursts','first_word words crc_payload begin_cycles end_cycles rx_before rx_after'.split());coverage=0
for b in bursts:
 assert b['first_word']==coverage and 0<b['words']<=128
 data=words[coverage*7:(coverage+b['words'])*7];assert zlib.crc32(data)==b['crc_payload'];coverage+=b['words']
assert coverage==36864 and len(bursts)==window['bursts']
trace=records('traces','tick reg len write status error rx_left tx_left offset valid'.split())
pre=records('preclose','trace_index phase begin_cycles end_cycles tick requested state sr cr1 cr2 cfg1 cfg2 error_before_close rx_size rx_remaining tx_remaining rx_offset cs offset_valid'.split())
payload=raw('payload');statuses=[];chunks=[]
for i,t in enumerate(trace):
 assert t['offset']+t['len']<=len(payload) and t['valid'] and not (t['status'] or t['error'] or t['rx_left'] or t['tx_left'])
 if not t['write']:
  items=[x for x in pre if x['trace_index']==i and x['phase']==2];assert len(items)==1;v=items[0]
  assert v['requested']==t['len']==v['rx_size']==v['rx_offset'] and not (v['rx_remaining'] or v['error_before_close'] or v['cs'] or v['sr']&64) and v['offset_valid']
 data=payload[t['offset']:t['offset']+t['len']]
 if not t['write'] and t['reg']==58:
  level=int.from_bytes(data,'little')&1023;assert level<512 and not data[1]&0x68;statuses.append(dict(tick=t['tick'],level=level,flags=data[1]))
 if not t['write'] and t['reg']==120:chunks.append(data)
first=scalar('cooperative_log_first_word');assert b''.join(chunks)==words[first*7:]
xyz=[];ts=[];previous=None;since=0;prefix=[]
for i in range(0,len(words),7):
 data=words[i:i+7];assert data[0].bit_count()%2==0;tag=data[0]>>3;count=(data[0]>>1)&3
 if tag==2:
  if previous is not None:assert count==(previous+1)%4
  previous=count;v=struct.unpack('<hhh',data[1:]);assert not any(x in (-32768,32767) for x in v);xyz.append(v);since+=1
  if len(xyz)%4096==0:prefix.append(dict(xyz=len(xyz),timestamps=len(ts),received=i//7+1))
 else:
  assert tag==4;value=int.from_bytes(data[1:5],'little')
  if ts:assert since==8 and 0<(value-ts[-1])%2**32<2**31
  ts.append(value);since=0
assert len(xyz)==32768 and len(ts)==4096
modbus=records('modbus','tick state xyz address count rx_bytes tx_count tx_complete_tick'.split())
assert [x['address'] for x in modbus]==[5000,0,1000,3000,5000,5000]
assert [x['state'] for x in modbus]==[1,2,2,2,2,4]
for x in modbus[1:5]:assert scalar('cooperative_start_tick')<=x['tick']<x['tx_complete_tick']<=scalar('cooperative_end_tick')
assert scalar('cooperative_command_io_denied')==1 and scalar('cooperative_command_snapshot_failures')==0 and scalar('tr2_production_poll_errors')==0
assert scalar('cooperative_fram_state')==3 and scalar('tr2_fifo_state')==3
assert scalar('cooperative_fram_crc_before')==scalar('cooperative_fram_crc_after')==zlib.crc32((p/'fram-before-capture.bin').read_bytes())
assert raw('persistent')==(p/'persistent-before.bin').read_bytes()
assert scalar('bench_log_refused')==scalar('bench_preclose_overflow')==0
hp0=json.loads((p/'hp-baseline-result.json').read_text());hp1=json.loads((p/'hp-capture-result.json').read_text());assert hp0['passed'] and hp1['passed'] and hp0['serialClosed'] and hp1['serialClosed'] and hp0['retries']==hp1['retries']==0
requests=hp0['requests']+hp1['requests'];assert len(requests)==6
# Independent CRC verification over every raw request/response, not a hash-only claim.
def crc(data):
 v=65535
 for b in data:
  v^=b
  for _ in range(8):v=(v>>1)^(0xA001 if v&1 else 0)
 return v
for q in requests:
 for field in ('request','response'):
  data=bytes.fromhex(q[field].replace('-',''));assert data[0]==1 and crc(data[:-2])==int.from_bytes(data[-2:],'little')
 assert q['passed'] and q['crcValid']
assert requests[0]['response']==requests[-1]['response']
assert requests[3]['response']==requests[4]['response']=='01-83-04-40-F3'
core=scalar('SystemCoreClock');assert core==160000000
ms=lambda cycles:cycles/(core/1000)
metrics=dict(fifo_max_level=window['max_level'],nominal_rate_words_per_second=26667*9/8,headroom_at_observed_max_ms=(512-window['max_level'])/(26667*9/8)*1000,max_gap_fifo_service_ms=ms(scalar('cooperative_service_max_gap_cycles')),max_fifo_poll_ms=ms(scalar('cooperative_max_cycles')),max_modbus_service_all_ms=ms(scalar('cooperative_modbus_max_cycles')),max_modbus_service_protected_ms=ms(scalar('cooperative_modbus_capture_max_cycles')),capture_with_cleanup_ms=(scalar('cooperative_end_tick')-scalar('cooperative_start_tick'))%2**32)
assert metrics['max_gap_fifo_service_ms']<metrics['headroom_at_observed_max_ms']
metrics['nominal_margin_ms']=metrics['headroom_at_observed_max_ms']-metrics['max_gap_fifo_service_ms']
summary=dict(code_sha='8bcb843',bin_sha256='b4301d6f3325c266edc19297413fd0885828e5625ec030a8acad340444b3fd9b',window=window,metrics=metrics,bursts_crc_verified=len(bursts),raw_spi_words_verified_last_window=(len(words)-first*7)//7,full_ram_words_sha256=hashlib.sha256(words).hexdigest(),timestamps_strictly_monotonic_but_no_global_loss_guarantee=True,prefixes=prefix,modbus=modbus,hp_requests=requests,fram_crc32_before_after=scalar('cooperative_fram_crc_before'),fram_sha256_before=hashlib.sha256((p/'fram-before-capture.bin').read_bytes()).hexdigest(),logical_persistent_bytes_unchanged=51914,command_io_denied=1,command_snapshot_failures=0,uart_error_and_event_drop_zero=True,generation=12,debugger_closed_during_hp_traffic=True,no_physical_command_write=True,restoration_needed=False,cumulative_firmware_kept=True)
for name,data in [('physical-independent-audit.json',summary),('decoded-bursts.json',bursts),('decoded-statuses-last-window.json',statuses),('decoded-modbus.json',modbus)]: (p/name).write_text(json.dumps(data,indent=2)+'\n')
print(json.dumps(dict(metrics=metrics,bursts=len(bursts),modbus_states=[x['state'] for x in modbus],prefixes=prefix),indent=2))
