from pathlib import Path
import struct,json,csv,hashlib,math
p=Path(Path('/tmp/tr2-m015-ip-proof').read_text());v=struct.unpack('<41I',(p/'burst-info.bin').read_bytes());keys='stage result restore_result core_hz spi_hz poll_count burst_count word_count accepted errors loss_flags tag_errors parity_errors gap_events saturations start_cycles end_cycles filled_status stopped_status final_status'.split();info=dict(zip(keys,v[:20]));info.update(saved=list(v[20:27]),configured=list(v[27:34]),restored=list(v[34:41]));assert info['result']==info['restore_result']==info['errors']==info['loss_flags']==info['tag_errors']==info['parity_errors']==info['gap_events']==info['saturations']==0 and info['accepted']==1 and info['word_count']==256 and info['burst_count']==2
(p/'burst-info.json').write_text(json.dumps(info,indent=2)+'\n')
tkeys='valid_mask phase method reg nbytes raw_length begin_cycles end_cycles hal_status error sr_before sr_after cfg1 cfg2 cs_before cs_during cs_after rx_left tx_left'.split()
def decode(b):
 v=struct.unpack('<19I16B',b);t=dict(zip(tkeys,v[:19]));t['tx_hex']=bytes(v[19:27][:t['raw_length']]).hex(' ').upper();t['rx_hex']=bytes(v[27:35][:t['raw_length']]).hex(' ').upper();t['duration_us']=(t['end_cycles']-t['begin_cycles'])/160;return t
n=struct.unpack('<I',(p/'config-trace-count.bin').read_bytes())[0];b=(p/'config-traces.bin').read_bytes();tr=[{'index':i,**decode(b[i*92:(i+1)*92])} for i in range(n)];assert n==40 and all(t['hal_status']==t['error']==t['rx_left']==t['tx_left']==0 for t in tr)
raw=b''.join(struct.pack('<19I',*(t[k] for k in tkeys))+bytes.fromhex(t['tx_hex']).ljust(8,b'\0')+bytes.fromhex(t['rx_hex']).ljust(8,b'\0') for t in tr);assert raw==b[:len(raw)];(p/'config-traces.json').write_text(json.dumps(tr,indent=2)+'\n')
with (p/'config-traces.csv').open('w') as f:w=csv.DictWriter(f,fieldnames=tr[0].keys(),lineterminator='\n');w.writeheader();w.writerows(tr)
rb=(p/'config-first-refusal.bin').read_bytes();assert rb==bytes(128);(p/'first-refusal.json').write_text(json.dumps({'valid':False,'all_other_fields_unpopulated':True,'reason':'No refusal occurred; zero fields are not measurements'},indent=2)+'\n')
mk='begin_cycles end_cycles status_before status_after hal_status error cs_before cs_during cs_after rx_left tx_left cfg1 cfg2'.split();mb=(p/'burst-meta.bin').read_bytes();metas=[dict(zip(mk,struct.unpack('<13I',mb[i*52:(i+1)*52]))) for i in range(2)]
for meta in metas:meta['duration_us']=(meta['end_cycles']-meta['begin_cycles'])/160;meta['payload_words_per_s_during_transfer']=128/(meta['duration_us']/1e6)
(p/'burst-meta.json').write_text(json.dumps(metas,indent=2)+'\n');pb=(p/'burst-polls.bin').read_bytes();polls=[dict(zip('begin_cycles end_cycles status level'.split(),struct.unpack('<4I',pb[i*16:(i+1)*16]))) for i in range(info['poll_count'])];(p/'polls.json').write_text(json.dumps(polls,indent=2)+'\n');assert all(polls[i]['level']>=polls[i-1]['level'] for i in range(1,len(polls)))
rx=(p/'burst-rx.bin').read_bytes();assert len(rx)==1794;(p/'burst-rx.hex').write_text(rx.hex(' ').upper()+'\n');assert bytes.fromhex((p/'burst-rx.hex').read_text())==rx
words=[];counter=None
for burst in range(2):
 frame=rx[burst*897:(burst+1)*897]
 for word in range(128):
  offset=1+7*word;tag=frame[offset];xyz=struct.unpack('<3h',frame[offset+1:offset+7]);cnt=(tag>>1)&3
  assert tag>>3==2 and tag.bit_count()%2==0 and (counter is None or cnt==(counter+1)&3);counter=cnt
  words.append({'index':len(words),'burst':burst,'word':word,'tag_hex':f'{tag:02X}','tag_id':tag>>3,'counter':cnt,'parity_even':True,'x_lsb':xyz[0],'y_lsb':xyz[1],'z_lsb':xyz[2]})
with (p/'xyz.csv').open('w') as f:w=csv.DictWriter(f,fieldnames=words[0].keys(),lineterminator='\n');w.writeheader();w.writerows(words)
# Lossless reconstruction of both raw frames, including observed command responses.
replayed=bytearray()
for burst in range(2):
 replayed.append(rx[burst*897])
 for w in words[burst*128:(burst+1)*128]:replayed.append(int(w['tag_hex'],16));replayed+=struct.pack('<3h',w['x_lsb'],w['y_lsb'],w['z_lsb'])
assert bytes(replayed)==rx
stats={}
for axis in ['x','y','z']:
 vals=[w[axis+'_lsb'] for w in words];mean=sum(vals)/256;stats[axis]={'min_lsb':min(vals),'max_lsb':max(vals),'mean_lsb':mean,'std_lsb':math.sqrt(sum((v-mean)**2 for v in vals)/256)}
assert not any(w[axis+'_lsb'] in [-32768,32767] for w in words for axis in ['x','y','z'])
assert [(x['status_before']&1023,x['status_after']&1023) for x in metas]==[(256,128),(128,0)]
assert all(x['hal_status']==x['error']==x['rx_left']==x['tx_left']==0 and [x['cs_before'],x['cs_during'],x['cs_after']]==[1,0,1] for x in metas)
analysis={'source_sha':'6c504a9d4035c2922d186a03350963bdac33379b','accepted':True,'word_count':256,'raw_rx_bytes':1794,'xyz_reconstruction_byte_equal':True,'trace_reconstruction_byte_equal':True,'trace_count':40,'populated_trace_bytes':len(raw),'trace_populated_sha256':hashlib.sha256(raw).hexdigest(),'rx_sha256':hashlib.sha256(rx).hexdigest(),'statistics_raw_lsb_only':stats,'poll_levels':[x['level'] for x in polls],'poll_status_hex':[f"{x['status']:04X}" for x in polls],'burst_times_us':[x['duration_us'] for x in metas],'two_burst_transfer_total_ms':sum(x['duration_us'] for x in metas)/1000,'fifo_enable_to_final_ms':(info['end_cycles']-info['start_cycles'])/160000,'configured_256_word_duration_ms':256/26667*1000,'odr_independently_measured':False,'loss_flags':0,'counter_discontinuities_observed':0,'counter_modulo4_cannot_exclude_all_losses':True,'continuous_v1_validated':False,'second_campaign':False}
(p/'analysis.json').write_text(json.dumps(analysis,indent=2)+'\n');print(json.dumps(analysis,indent=2))
