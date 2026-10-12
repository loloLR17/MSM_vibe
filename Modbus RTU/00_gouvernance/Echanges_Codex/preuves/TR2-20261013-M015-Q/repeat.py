from pathlib import Path
import json,struct,sys,time,hashlib,importlib.util
import hardware_second as h
P=Path(__file__).parent
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-M015-Q')
i=json.loads((P/'initial.json').read_text());elf=Path(i['control_elf']);image=Path(i['control_bin'])
name=sys.argv[1];assert name in ('repeat1','repeat2','repeat3')
if name!='repeat1':assert json.loads((P/('repeat'+str(int(name[-1])-1)+'-qualification.json')).read_text())['bounded_acceptance']
assert hashlib.sha256(image.read_bytes()).hexdigest()==i['control_bin_sha256']
assert hashlib.sha256(elf.read_bytes()).hexdigest()==i['control_elf_sha256']
identity='x/3wx 0x0BFA0700\nx/wx 0xE0044000\nif *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157\n quit 3\nend\n'
prefix=h.win(P).replace('\\','/')
h.debug(name+'-identity-readback',identity+f'dump binary memory {prefix}/{name}-flash.bin 0x08000000 {hex(0x08000000+image.stat().st_size)}\np bench_result\nx/2wx 0xE000ED28\n',elf)
assert (P/(name+'-flash.bin')).read_bytes()==image.read_bytes()
h.debug(name+'-mcu-reset',identity+'monitor reset\n',elf)
time.sleep(.2)
h.debug(name+'-single-trigger','p bench_ready\np bench_uid\np bench_core_hz\np bench_result\nx/2wx 0xE000ED28\nif bench_ready != 1 || bench_core_hz != 16000000 || bench_uid[0] != 0x001B0046 || bench_uid[1] != 0x38425018 || bench_uid[2] != 0x20333157\n quit 4\nend\nif bench_result.stage != 0 || bench_result.count != 0 || *(unsigned int *)0xE000ED28 != 0 || *(unsigned int *)0xE000ED2C != 0\n quit 5\nend\nset var bench_command=0x4D31354B\n',elf)
time.sleep(1.7)
body='p bench_result\np bench_trace_count\np bench_payload_count\np bench_preclose_count\np bench_uid\np bench_core_hz\np official_ctrl10\nx/2wx 0xE000ED28\nif bench_trace_count > 1024 || bench_payload_count > 65536 || bench_preclose_count > 2048 || bench_result.count > 1024 || official_text_count > 2048\n quit 6\nend\n'
for label,start,end in [('result','&bench_result','(&bench_result+1)'),('traces','bench_trace','((char *)bench_trace)+bench_trace_count*sizeof(BenchTrace)'),('payload','bench_payload','bench_payload+bench_payload_count'),('preclose','bench_preclose','((char *)bench_preclose)+bench_preclose_count*sizeof(BenchPreclose)'),('overflow','&bench_preclose_overflow','(&bench_preclose_overflow+1)'),('log-refused','&bench_log_refused','(&bench_log_refused+1)'),('official-words','official_words','official_words+bench_result.count*7'),('ctrl10','&official_ctrl10','(&official_ctrl10+1)')]:
 body+=f'dump binary memory {prefix}/{name}-{label}.bin {start} {end}\n'
h.debug(name+'-capture',body,elf)
import subprocess
subprocess.run([sys.executable,str(P/'analyze_repeat.py'),name],check=True)
