from pathlib import Path
import json,struct,hashlib,csv,statistics,datetime,subprocess
import sys
r=Path(sys.argv[1]).resolve() if len(sys.argv)>1 else Path.cwd();e=r/'Modbus RTU/00_gouvernance/Echanges_Codex';q=e/'preuves/TR2-20261013-M015-Q'
stamp=datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ');p=Path('/home/lolo/dev/msm/reports')/('TR2-mission015-R-'+stamp);p.mkdir();Path('/tmp/tr2-m015-r-proof').write_text(str(p))
trraw=bytes.fromhex((q/'window4096-traces.bin.hex').read_text());prraw=bytes.fromhex((q/'window4096-preclose.bin.hex').read_text());payload=bytes.fromhex((q/'window4096-payload.bin.hex').read_text());words=bytes.fromhex((q/'window4096-official-words.bin.hex').read_text())
tkeys=['tick','reg','len','write','status','error','rx_left','tx_left','offset','valid'];pk=['trace_index','phase','begin_cycles','end_cycles','tick','requested','state','sr','cr1','cr2','cfg1','cfg2','error_before_close','rx_size','rx_remaining','tx_remaining','rx_offset','cs','offset_valid']
tr=[dict(zip(tkeys,struct.unpack('<10I',trraw[i:i+40]))) for i in range(0,len(trraw),40)];pr=[dict(zip(pk,struct.unpack('<19I',prraw[i:i+76]))) for i in range(0,len(prraw),76)]
assert tr==json.loads((q/'window4096-trace-records.json').read_text())
assert b''.join(payload[t['offset']:t['offset']+t['len']] for t in tr if t['reg']==120 and not t['write'])==words
assert all(x['valid'] and not x['status'] and not x['error'] for x in tr)
fifo=[i for i,t in enumerate(tr) if t['reg']==120 and not t['write']];assert len(fifo)==20
by={i:[x for x in pr if x['trace_index']==i] for i in range(len(tr))}
def dt(b,a):return ((b-a)&0xFFFFFFFF)/16000 # ms at observed16MHz
intervals=[]
for first,last in zip(fifo,fifo[1:]):
 b=by[first][0]['begin_cycles'];end=by[last][0]['begin_cycles'];span=dt(end,b)
 cats={k:0. for k in ['FIFO_payload_ms','FIFO_command_ms','status_HAL_ms','config_HAL_ms']}
 for i in range(first,last):
  for x in by[i]:
   dur=dt(x['end_cycles'],x['begin_cycles']);key=('FIFO_command_ms' if x['phase']==1 else 'FIFO_payload_ms') if tr[i]['reg']==120 else ('status_HAL_ms' if tr[i]['reg']==58 else 'config_HAL_ms');cats[key]+=dur
 rec={'first_trace':first,'next_trace':last,'interval_ms':span,**cats,'unobserved_residual_ms':span-sum(cats.values()),'post_FIFO_to_status_envelope_ms':dt(by[first+1][0]['begin_cycles'],by[first][-1]['end_cycles'])}
 intervals.append(rec)
with (p/'service-intervals.csv').open('w',newline='') as f:
 w=csv.DictWriter(f,fieldnames=list(intervals[0]),lineterminator='\n');w.writeheader();w.writerows(intervals)
st=[]
for i,t in enumerate(tr):
 if t['reg']==58 and not t['write']:
  b=payload[t['offset']:t['offset']+2];st.append({'trace_index':i,'begin_cycles':by[i][-1]['begin_cycles'],'tick':t['tick'],'level':int.from_bytes(b,'little')&1023,'flags':b[1],'raw_hex':b.hex()})
# Choose last status before first and last burst; before overflow/latch.
s1=next(x for x in st if x['trace_index']==fifo[0]-1);s2=next(x for x in st if x['trace_index']==fifo[-1]-1)
assert not(s1['flags']&0x68 or s2['flags']&0x68)
ms=dt(s2['begin_cycles'],s1['begin_cycles']);drained=19*128;inferred=drained+s2['level']-s1['level']
ts=[];xyz_between=[];since=0
for i in range(0,len(words),7):
 b=words[i:i+7];tag=b[0]>>3
 if tag==2:since+=1
 elif tag==4:
  ts.append(int.from_bytes(b[1:5],'little'))
  if len(ts)>1:xyz_between.append(since)
  since=0
assert all(x==8 for x in xyz_between)
ts_ticks=(ts[-1]-ts[0])&0xFFFFFFFF;relative_odr=8*(len(ts)-1)/(ts_ticks*12.5e-6)
summary={'initial_head':subprocess.check_output(['git','rev-parse','HEAD'],cwd=r,text=True).strip(),'historical_code_sha':'76b4a17577171c0009ada7e0c67e387286d282dc','Q_bin_sha256':'d76a6104a2adc19f77e3b9849e32b9d52312dfd2840eb01f76eb4749cf687b09','clock':{'observed_core_hz':16000000,'SPI3_source':'PCLK3; MSI16, AHB/APB3 div1','cfg1_values':sorted(set(hex(x['cfg1']) for x in pr)),'prescaler_MBR':sorted(set((x['cfg1']>>28)&7 for x in pr)),'divider':8,'calculated_SPI_hz':2000000,'wire_frequency_measured':False,'CPU_SPI_coupled':True},'means_ms':{key:statistics.mean(x[key] for x in intervals) for key in list(intervals[0])[2:]},'interval_min_max_ms':[min(x['interval_ms'] for x in intervals),max(x['interval_ms'] for x in intervals)],'sensor_timestamp_estimate':{'count':len(ts),'delta_ticks':ts_ticks,'nominal_tick_us':12.5,'XYZ_between_each_TS':8,'relative_ODR_Hz':relative_odr,'not_independent_ODR':'INTERNAL_FREQ_FINE absent and no calibrated clock/external DRDY measurement'},'FIFO_balance_estimate':{'first':s1,'last':s2,'elapsed_DWT_ms':ms,'drained_words':drained,'inferred_arrivals_words':inferred,'inferred_total_words_per_second':inferred*1000/ms,'inferred_XYZ_Hz_assuming_8_to_1':inferred*1000/ms*8/9,'not_independent':'reads asynchronous, quantization and oscillator calibration unknown; pre-overflow status based estimate'},'required_bytes_per_second_nominal':(80000/3)*7*9/8,'required_bytes_per_second_timestamp_nominal_tick':relative_odr*7*9/8,'SPI_ideal_payload_capacity_bytes_per_second':2000000/8,'SPI_ideal_128word_frame_capacity_bytes_per_second':(2000000/8)*896/897,'measured_scheduled_payload_bytes_per_second':19*896/(sum(x['interval_ms'] for x in intervals)/1000),'margin_budget_ms_per_128words_nominal':896/210000*1000,'pure_wire_command_payload_ms':897*8/2000000*1000,'decision':'A suffices: wire has theoretical margin; full service spends too much outside FIFO payload; no CPU-only attribution and no conditional physical test needed.'}
summary['allowed_overhead_above_measured_payload_ms']=summary['margin_budget_ms_per_128words_nominal']-summary['means_ms']['FIFO_payload_ms']
summary['sources']={x.name:hashlib.sha256(x.read_bytes()).hexdigest() for x in q.glob('window4096-*') if x.suffix in ['.json','.hex']}
(p/'summary.json').write_text(json.dumps(summary,indent=2)+'\n');(p/'status-timeline.json').write_text(json.dumps(st,indent=2)+'\n');(p/'analysis.py').write_bytes(Path(__file__).read_bytes())
print(json.dumps({k:v for k,v in summary.items() if k!='sources'},indent=2));print('proof',p)
