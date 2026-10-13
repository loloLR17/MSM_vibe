from pathlib import Path
import json,struct,sys,time,hashlib,subprocess
import hardware_second as h
P=Path(__file__).parent
name='cpu160spi4';elf=P/name/'iis3dwb_reference.elf'
art=json.loads((P/'artifacts.json').read_text())[name]
assert hashlib.sha256(elf.read_bytes()).hexdigest()==art[elf.name]['sha256']
assert (P/(name+'-flash.bin')).read_bytes()==(P/name/'iis3dwb_reference.bin').read_bytes()
assert not (P/(name+'-capture.gdb')).exists(), 'Capture already consumed'
prefix=h.win(P).replace('\\','/')
h.debug(name+'-single-trigger-corrected','p bench_ready\np bench_uid\np bench_core_hz\np bench_result\nx/2wx 0xE000ED28\nif bench_ready != 1 || bench_core_hz != 160000000 || bench_uid[0] != 0x001B0046 || bench_uid[1] != 0x38425018 || bench_uid[2] != 0x20333157\n quit 4\nend\nif bench_result.stage != 0 || bench_result.count != 0 || *(unsigned int *)0xE000ED28 != 0 || *(unsigned int *)0xE000ED2C != 0\n quit 5\nend\np SystemCoreClock\nx/10wx 0x46020C00\nx/wx 0x46020CE8\nx/wx 0x4602080C\nx/wx 0x40022000\nx/6wx 0x46002000\nif SystemCoreClock != 160000000 || (*(unsigned int *)0x46020C00 & 0x400) == 0 || (*(unsigned int *)0x46020CE8 & 0x18) != 0x10 || (*(unsigned int *)0x46020C24 & 0x70) != 0 || (*(unsigned int *)0x4602080C & 0x38000) != 0x38000 || (*(unsigned int *)0x40022000 & 0xF) != 4 || (*(unsigned int *)0x46002008 & 0x70000000) != 0x10000000\n quit 7\nend\nset var bench_command=0x4D31354B\n',elf)
time.sleep(1.7)
body='p bench_result\np bench_trace_count\np bench_payload_count\np bench_preclose_count\np bench_uid\np bench_core_hz\np official_ctrl10\nx/2wx 0xE000ED28\nif bench_trace_count > 1024 || bench_payload_count > 65536 || bench_preclose_count > 2048 || bench_result.count > 4736 || official_text_count > 2048\n quit 6\nend\n'
for label,start,end in [('result','&bench_result','(&bench_result+1)'),('traces','bench_trace','((char *)bench_trace)+bench_trace_count*sizeof(BenchTrace)'),('payload','bench_payload','bench_payload+bench_payload_count'),('preclose','bench_preclose','((char *)bench_preclose)+bench_preclose_count*sizeof(BenchPreclose)'),('overflow','&bench_preclose_overflow','(&bench_preclose_overflow+1)'),('log-refused','&bench_log_refused','(&bench_log_refused+1)'),('official-words','official_words','official_words+bench_result.count*7'),('ctrl10','&official_ctrl10','(&official_ctrl10+1)'),('window','&fifo_window','(&fifo_window+1)')]:
 body+=('if bench_result.count > 0\n' if label=='official-words' else '')+f'dump binary memory {prefix}/{name}-{label}.bin {start} {end}\n'+('end\n' if label=='official-words' else '')
body+=f'dump binary memory {prefix}/{name}-RCC.bin 0x46020C00 0x46020CF0\n'
body+=f'dump binary memory {prefix}/{name}-PWR.bin 0x46020800 0x46020820\n'
body+=f'dump binary memory {prefix}/{name}-FLASH-ACR.bin 0x40022000 0x40022004\n'
h.debug(name+'-capture',body,elf)
if not (P/(name+'-official-words.bin')).exists():
 assert struct.unpack('<I',(P/(name+'-result.bin')).read_bytes()[12:16])[0]==0
 (P/(name+'-official-words.bin')).write_bytes(b'')
subprocess.run([sys.executable,str(P/'analyze_window.py'),name],check=True)
