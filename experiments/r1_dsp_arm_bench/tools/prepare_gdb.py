"""Generate guarded scripts ONLY, never connect/flash/reset/trigger a target.
Use exclusively after physical preflight of TR2-002 and its qualified ST-LINK.
Run and capture scripts halt only at WAIT and after the terminal breakpoint.
"""
from pathlib import Path
import argparse,subprocess,json,hashlib
ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--image',type=Path,required=True);ap.add_argument('--scripts',type=Path,required=True);ap.add_argument('--capture',type=Path,required=True);a=ap.parse_args()
if not a.image.is_file():ap.error('Missing firmware image')
a.scripts.mkdir(parents=True,exist_ok=False);a.capture.mkdir(parents=True,exist_ok=False)
def target(n):
 s=subprocess.check_output(['wslpath','-w',str((a.capture/n).resolve())],text=True).strip().replace('\\','/')
 if any(c.isspace() or c=='"' for c in s):raise ValueError('GDB dump path must have no whitespace or quotes')
 return s
pre='set pagination off\nset confirm off\nset remotetimeout 10\ntarget extended-remote localhost:61236\n'
identity='''x/3wx 0x0BFA0700
if *(unsigned int*)0x0BFA0700 != 0x001B0046 || *(unsigned int*)0x0BFA0704 != 0x38425018 || *(unsigned int*)0x0BFA0708 != 0x20333157
 detach
 quit 3
end
'''
backup=pre+identity+'dump binary memory '+target('initial-flash-2MiB.bin')+' 0x08000000 0x08200000\ndetach\nquit\n'
wait=pre+identity+'''p r1_result
if r1_result.stage != 2 || r1_result.error || r1_result.clock_hz != 160000000 || r1_command != 0
 detach
 quit 4
end
'''
run=wait+'''thbreak r1_bench_finished
set var r1_command=0x52314131
continue
p r1_result
'''
for name,expr,end in [('result','&r1_result','(&r1_result+1)'),('blocks','r1_block_cycles','((char*)r1_block_cycles)+sizeof(r1_block_cycles)'),('emissions','r1_emit_cycles','((char*)r1_emit_cycles)+sizeof(r1_emit_cycles)'),('overhead','r1_overhead_cycles','((char*)r1_overhead_cycles)+sizeof(r1_overhead_cycles)')]:
 run+='dump binary memory '+target(name+'.bin')+' '+expr+' '+end+'\n'
run+='''if r1_result.stage != 4 || r1_result.error || r1_result.complete_magic != 0x4431304F
 detach
 quit 5
end
detach
quit
'''
(a.scripts/'backup-readonly.gdb').write_text(backup)
(a.scripts/'wait-readonly.gdb').write_text(wait+'dump binary memory '+target('programmed-prefix.bin')+' 0x08000000 '+hex(0x08000000+a.image.stat().st_size)+'\ndetach\nquit\n')
(a.scripts/'run-to-terminal.gdb').write_text(run)
(a.scripts/'manifest.json').write_text(json.dumps(dict(scripts_prepared_not_executed=True,authorized_probe_only='0032001F3235510C37333439',excluded_probe='003500463235510B37333439',gdb_port=61236,image=str(a.image.resolve()),bytes=a.image.stat().st_size,sha256=hashlib.sha256(a.image.read_bytes()).hexdigest(),required_preflight='Fresh ST-LINK SN and UID, bench physically safe; initial image backup/recovery verified; qualified flash verify before WAIT/run. Never use the link-only R1-DSP ELF.'),indent=2)+'\n')
print('Guarded scripts prepared only; no target accessed')
