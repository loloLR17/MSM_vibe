import importlib.util,subprocess,sys,struct,zlib
from pathlib import Path
spec=importlib.util.spec_from_file_location('ref',Path(__file__).parents[1]/'tools/analyze_reference.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
all_frames=subprocess.check_output([sys.argv[1]]);b=all_frames[:32];assert m.frame(b)==(7,10000000000)
wrong=bytearray(b);wrong[7]^=1
try:m.frame(wrong);raise AssertionError('CRC accepted')
except ValueError:pass
wrong=bytearray(b);struct.pack_into('<I',wrong,4,0x004a003d);struct.pack_into('<I',wrong,28,zlib.crc32(wrong[:28]))
try:m.frame(wrong);raise AssertionError('Wrong UID accepted')
except ValueError:pass
second=bytearray(b);struct.pack_into('<IQ',second,16,8,11600000000);struct.pack_into('<I',second,28,zlib.crc32(second[:28]))
d=dict(high_resolution=True,frequency=10000000,queries=[dict(send_tick=100,receive_tick=10100,frame_hex=b.hex()),dict(send_tick=100000100,receive_tick=100010100,frame_hex=second.hex())]);r=m.bounds(d);assert r['nominal_160MHz_inside_bounds'] and not r['retroactive_hour_qualified']
for field,value in [('frequency',0),('high_resolution',False)]:
 x={**d,field:value}
 try:m.bounds(x);raise AssertionError(field)
 except ValueError:pass
x={**d,'queries':[d['queries'][0],{**d['queries'][1],'send_tick':5000}]}
try:m.bounds(x);raise AssertionError('overlap')
except ValueError:pass
x={**d,'queries':[d['queries'][0],{**d['queries'][1],'frame_hex':b.hex()}]}
try:m.bounds(x);raise AssertionError('reset/replay')
except ValueError:pass
print('C frame vs independent PythonCRC; wrongUID/CRC/replay/reset/overlap/QPC refusal; exact interval bounds PASS (HOST ONLY)')

a=all_frames[32:];x=m.active_frame(a);assert x['session']==99 and x['accepted']==1234
b=bytearray(a);struct.pack_into('<I',b,16,4);struct.pack_into('<Q',b,20,3605000000);struct.pack_into('<I',b,40,95000000);struct.pack_into('<I',b,44,zlib.crc32(b[:44]))
d=dict(high_resolution=True,frequency=10000000,minimum_seconds=3601,expected_run=5,failure=None,first=dict(send_tick=0,receive_tick=100,frame_hex=a.hex()),second=dict(send_tick=36010000102,receive_tick=36010001000,frame_hex=b.hex()))
r=m.active_bounds(d);assert r['independent_inner_interval_s']>=3601 and not r['physical_hour_gate_qualified']
for bad in [dict(d,second=dict(d['second'],send_tick=36000000000)),dict(d,failure='earlySTOP')]:
 try:m.active_bounds(bad);raise AssertionError('bad interval/failure')
 except ValueError:pass
b[28]=0;struct.pack_into('<I',b,44,zlib.crc32(b[:44]));bad=dict(d,second=dict(d['second'],frame_hex=b.hex()))
try:m.active_bounds(bad);raise AssertionError('inactive')
except ValueError:pass
print('QPC2 C/Python frame and causal INNER interval; too-short/earlySTOP/inactive refuse PASS (MODEL ONLY)')
