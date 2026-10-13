"""Read every successfully written byte of a failed run, with independent oracle.
No write/reset/retry; requires matching archived ELF and terminal stage99.
"""
from pathlib import Path
import argparse,json,sys,hashlib
p=argparse.ArgumentParser();p.add_argument('--proof',type=Path,required=True);p.add_argument('--elf',type=Path,required=True);p.add_argument('--run',type=int,required=True);a=p.parse_args();P=a.proof;sys.path.insert(0,str(P));from hardware import debug,IDENTITY,win
name=f'run-{a.run:02d}';m=json.loads((P/(name+'-result.json')).read_text())['metrics'];written=m['load_written'];assert m['load_stage']==99 and written%512==0
body=IDENTITY+f'if load_stage != 99 || load_producer_enabled != 0 || load_written != {written}\n quit 4\nend\n'
parts=[]
for off in range(0,written,65536):
 count=min(65536,written-off);f=P/f'{name}-prefix-{off//65536:04d}.bin';parts.append((off,count,f))
 body+=f'set $ok=transfer(0,rx,{2048+off//512},{count})\nprintf "READ offset={off} bytes={count} status=%u\\n", $ok\nif $ok != 1\n quit 7\nend\ndump binary memory '+win(f)+f' rx rx+{count}\n'
body+='p load_stage\np load_error\np/x hsd2.ErrorCode\np/x load_last_r1\nx/2wx 0xE000ED28\n'
debug(name+'-prefix-readonly',a.elf,body,timeout=600)
checks=[];bad=0;first=None
for off,count,f in parts:
 b=f.read_bytes();assert len(b)==count
 expected=bytes((x*37+(x>>8)+(x>>16)+(x>>24)*113)&255 if x<256003200 else 0xa5 for x in range(off,off+count))
 mismatches=[off+i for i,(x,y) in enumerate(zip(b,expected)) if x!=y]
 bad+=len(mismatches)
 if mismatches and first is None:first=mismatches[0]
 checks.append({'offset':off,'bytes':count,'sha256_read':hashlib.sha256(b).hexdigest(),'sha256_expected':hashlib.sha256(expected).hexdigest(),'mismatches':len(mismatches)})
result={'run':a.run,'scope':'all bytes actually written, independent byte comparison; no new write/reset/retry','verified_bytes':written,'mismatches':bad,'first_mismatch':first,'checks':checks,'firmware_verified_counters_not_modified':True}
(P/(name+'-prefix-verification.json')).write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result),flush=True)
