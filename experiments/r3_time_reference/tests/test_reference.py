import importlib.util,subprocess,sys,struct,zlib
from pathlib import Path
spec=importlib.util.spec_from_file_location('ref',Path(__file__).parents[1]/'tools/analyze_reference.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
b=subprocess.check_output([sys.argv[1]]);assert m.frame(b)==(7,10000000000)
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
