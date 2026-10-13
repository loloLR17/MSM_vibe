from pathlib import Path
import sys,json,hashlib,time
P=Path(Path('/tmp/m016-load-proof').read_text());sys.path.insert(0,str(P));from hardware import *
a=json.loads((P/'run-analysis.json').read_text());written=a['raw_metrics']['load_written'];assert written==1310720
elf=Path('/home/lolo/dev/msm/reports/M016-1bit-load-build/sdmmc_reference.elf')
body=IDENTITY+'''p load_stage
p load_error
p load_written
if load_stage != 99 || load_error != 3 || load_producer_enabled != 0 || load_written != 1310720
 quit 4
end
'''
for i in range(written//65536):
 body+=f'set $read_ok=transfer(0,rx,{2048+i*128},65536)\nprintf "READ_CHUNK {i} result=%u\\n", $read_ok\nif $read_ok != 1\n quit 7\nend\ndump binary memory '+win(P/f'prefix-read-{i:03d}.bin')+' rx rx+65536\n'
body+='p load_stage\np load_error\np/x hsd2.ErrorCode\np/x load_last_r1\nx/2wx 0xE000ED28\n'
debug('integral-written-prefix-readonly',elf,body,timeout=180)
results=[];mismatches=0;first=None
for i in range(written//65536):
 b=(P/f'prefix-read-{i:03d}.bin').read_bytes();start=i*65536
 expected=bytes((o*37+(o>>8)+(o>>16)+(o>>24)*113)&255 for o in range(start,start+65536))
 bad=[j for j,(x,y) in enumerate(zip(b,expected)) if x!=y];mismatches+=len(bad)
 if bad and first is None:first=start+bad[0]
 assert len(b)==65536
 results.append({'chunk':i,'lba':2048+i*128,'bytes':len(b),'sha256_read':hashlib.sha256(b).hexdigest(),'sha256_expected':hashlib.sha256(expected).hexdigest(),'mismatches':len(bad)})
(P/'integral-prefix-verification.json').write_text(json.dumps({'scope':'all bytes actually successfully written by the one failed load run; no new write or retry','verified_bytes':written,'chunks':len(results),'mismatches':mismatches,'first_mismatch':first,'checks':results,'original_firmware_verified_counter_remains_zero':True},indent=2)+'\n');print('INTEGRAL_WRITTEN_PREFIX',written,'bytes; mismatches',mismatches,flush=True)
