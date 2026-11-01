from pathlib import Path
import json,re,struct,collections,hashlib,datetime
P=Path(__file__).parent
log=(P/'hardware/TR2-001-boot-calibration-RAM-only.log').read_text()
line=next(x for x in log.splitlines() if 'calibration_residual_us =' in x)
values={k:int(v) for k,v in re.findall(r'(\w+) = (\d+)',line)}
raw=(P/'calibration-first-raw.bin').read_bytes();assert len(raw)==values['raw_words']*7
records=[raw[i:i+7] for i in range(0,len(raw),7)];tags=collections.Counter(b[0]>>3 for b in records)
assert set(tags)<= {2,4} and all(b[0].bit_count()%2==0 for b in records)
xyz=[struct.unpack('<hhh',b[1:]) for b in records if b[0]>>3==2]
assert all(v not in (-32768,32767) for row in xyz for v in row)
intervals=[];n=None
for b in records:
 if b[0]>>3==4:
  if n is not None:intervals.append(n)
  n=0
 elif n is not None:n+=1
assert intervals and set(intervals)=={8}
bs=(P/'calibration-brackets.bin').read_bytes();brackets=list(struct.iter_unpack('<QQII',bs));assert len(brackets)==values['brackets']
a=brackets[10];z=brackets[-1];span_us=(z[0]+z[1]-a[0]-a[1])//2;span_ticks=(z[2]-a[2])&0xffffffff
residuals=[]
for before,after,ticks,index in brackets[10:]:
 predicted=(a[0]+a[1])//2+((ticks-a[2])&0xffffffff)*span_us//span_ticks
 residuals.append(abs((before+after)//2-predicted))
assert (span_us,span_ticks,max(residuals))==(values['span_us'],values['span_ticks'],values['calibration_residual_us'])
t=json.loads((P/'BOOT_RETAINED_TRANSACTIONS.json').read_text());assert any(not r['write'] and r['reg']=='0x7' and r['payload']=='00010a86' for r in t)
assert any(not r['write'] and r['reg']=='0xf' and r['payload'].startswith('7ba00044') for r in t)
assert any(not r['write'] and r['reg']=='0x19' and r['payload']=='20' for r in t)
assert any(r['write'] and r['reg']=='0xa' and r['payload']=='80' for r in t)
assert any(not r['write'] and r['reg']=='0xa' and r['payload']=='80' for r in t)
out=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),boot_calibration_observed=True,no_new_commanded_acquisition=True,approximate_nominal_calibration_duration_us=values['end_us']-values['begin_us'],total_XYZ=52442,total_FIFO_words=values['words'],anchors=values['anchors'],brackets=values['brackets'],firmware_error=values['error'],counter_mismatch=values['counter_mismatch'],reserved_nonzero=values['reserved_nonzero'],calibration_residual_us=values['calibration_residual_us'],independent_recomputed_max_residual_us=max(residuals),guard_us=100,raw_prefix_words=len(records),raw_prefix_XYZ=len(xyz),raw_prefix_timestamps=tags[4],raw_prefix_parity_valid=True,complete_intertimestamp_intervals=len(intervals),all_complete_intervals_8_XYZ=True,axis_min_counts=[min(row[k] for row in xyz) for k in range(3)],axis_max_counts=[max(row[k] for row in xyz) for k in range(3)],axis_distinct_counts=[len(set(row[k] for row in xyz)) for k in range(3)],raw_prefix_sha256=hashlib.sha256(raw).hexdigest(),configured_full_scale='±2g (code0), unchanged',SPI3_profile='4MHz nominal / mode3 / existing duplex',observed_active_FIFO_CTRL4='0x86',observed_stopped_FIFO_CTRL4='0x80',WHO_from_retained_trace='0x7B',sensor_identity='same borrowed module according to user; WHO is not unique serial',limits=['Not ±8g qualified','Not long acquisition or SD coexistence','Board, wiring, power, software and full scale differ from TR2-002; MCU defect not isolated','Raw prefix only; not all native XYZ bytes retained','FRAM whole content not reread or hash-verified in this diagnostic'])
(P/'BOOT_COMPARISON.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out,indent=2))
