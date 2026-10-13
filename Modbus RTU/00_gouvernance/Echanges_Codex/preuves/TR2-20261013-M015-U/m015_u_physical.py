from pathlib import Path
import subprocess,time,hashlib,json,struct,importlib.util
P=Path('/tmp/TR2-mission015-U');r=Path('/home/lolo/dev/msm/projects/MSM_vibe-M015-U')
spec=importlib.util.spec_from_file_location('h','/home/lolo/dev/msm/reports/TR2-mission015-J-20261012T184145Z/hardware.py');h=importlib.util.module_from_spec(spec);spec.loader.exec_module(h);h.P=P
sha=subprocess.check_output(['git','rev-parse','HEAD'],cwd=r).decode().strip();assert sha==subprocess.check_output(['git','rev-parse','origin/main'],cwd=r).decode().strip()
files=subprocess.check_output(['git','diff-tree','--no-commit-id','--name-only','-r','-z',sha],cwd=r).decode().strip('\0').split('\0')
for f in files:assert subprocess.check_output(['git','show','origin/main:'+f],cwd=r)==(r/f).read_bytes()
(P/'checkpoint-readback.json').write_text(json.dumps({'sha':sha,'files':files,'byte_equal':True},indent=2)+'\n')
elf=P/'build-st-window/tr2_stm32_p11c.elf';image=P/'build-st-window/tr2_stm32_p11c.bin'
a=json.loads((P/'software-results.json').read_text());assert hashlib.sha256(image.read_bytes()).hexdigest()==a['artifacts']['build-st-window/tr2_stm32_p11c.bin']['sha256']
old=Path('/home/lolo/dev/msm/reports/TR2-mission014-H-20261012T073729Z/build-production-live');oldelf=old/'tr2_stm32_p11c.elf'
prefix=h.win(P).replace('\\','/')
identity='x/3wx 0x0BFA0700\nif *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157\n quit 3\nend\n'
h.debug('identity-before-flash',oldelf,identity+f'dump binary memory {prefix}/persistent-candidate-before.bin production_runtime.candidate (production_runtime.candidate+51914)\n')
span=((max(image.stat().st_size,(old/'tr2_stm32_p11c.bin').stat().st_size)+8191)//8192)*8192
restore=P/'restore-pages.bin';restore.write_bytes((P/'flash-before.bin').read_bytes()[:span]);assert restore.read_bytes()[:(old/'tr2_stm32_p11c.bin').stat().st_size]==(old/'tr2_stm32_p11c.bin').read_bytes()
def flash(name,f,source):
 cmd=[h.CLI,'-c','port=SWD','sn='+h.SN,'freq=8000','-w',h.win(f),'0x08000000','-v','-rst']
 with (P/(name+'-command.json')).open('x') as out:json.dump({'command':cmd,'source_sha':source,'sha256':hashlib.sha256(f.read_bytes()).hexdigest(),'normal_flash_only':True},out,indent=2)
 x=subprocess.run(cmd,capture_output=True,timeout=50);(P/(name+'.log')).write_bytes(x.stdout+x.stderr);x.check_returncode();assert b'Download verified successfully' in x.stdout
attempt=False
try:
 attempt=True;flash('flash-integrated',image,sha)
 h.debug('integrated-readback-reset',elf,identity+f'dump binary memory {prefix}/flash-integrated-full.bin 0x08000000 0x08200000\nmonitor reset\n')
 read=(P/'flash-integrated-full.bin').read_bytes();assert read[:image.stat().st_size]==image.read_bytes();assert read[span:]==(P/'flash-before.bin').read_bytes()[span:]
 time.sleep(1)
 h.debug('integrated-preflight',elf,identity+'''p bench_ready
p tr2_fifo_state
p tr2_fifo_fault
p tr2_fifo_fram_status
p bench_core_hz
p SystemCoreClock
x/10wx 0x46020C00
x/wx 0x46020CE8
x/wx 0x4602080C
x/wx 0x40022000
x/6wx 0x46002000
x/2wx 0xE000ED28
if bench_ready != 1 || tr2_fifo_state != 1 || tr2_fifo_fram_status != 1 || SystemCoreClock != 160000000 || bench_core_hz != 160000000
 quit 4
end
if (*(unsigned int *)0x46020CE8 & 0x18) != 0x10 || (*(unsigned int *)0x40022000 & 15) != 4 || (*(unsigned int *)0x46002008 & 0x70000000) != 0x10000000
 quit 7
end
'''+f'dump binary memory {prefix}/fram-before-capture.bin tr2_fifo_fram_backup (tr2_fifo_fram_backup+262144)\n')
 assert hashlib.sha256((P/'fram-before-capture.bin').read_bytes()).hexdigest()=='d732acc557152180fc394097d6a5269ea2f45c1da387c8643f512318adf92fce', 'FRAM differs historical12; STOP before acquisition'
 h.debug('unique-trigger',elf,identity+'''if bench_ready != 1 || tr2_fifo_state != 1 || bench_result.stage != 0 || bench_trace_count != 0
 quit 8
end
set var bench_command=0x4D31354B
''')
 time.sleep(1.7)
 body='p tr2_fifo_state\np tr2_fifo_fault\np tr2_fifo_fram_status\np bench_result\np fifo_window\np bench_uid\np official_ctrl10\nx/2wx 0xE000ED28\n'
 body+='if bench_trace_count > 1024 || bench_payload_count > 65536 || bench_preclose_count > 2048 || bench_result.count > 4736\n quit 6\nend\n'
 for label,start,end in [('result','&bench_result','(&bench_result+1)'),('traces','bench_trace','((char *)bench_trace)+bench_trace_count*sizeof(BenchTrace)'),('payload','bench_payload','bench_payload+bench_payload_count'),('preclose','bench_preclose','((char *)bench_preclose)+bench_preclose_count*sizeof(BenchPreclose)'),('overflow','&bench_preclose_overflow','(&bench_preclose_overflow+1)'),('log-refused','&bench_log_refused','(&bench_log_refused+1)'),('official-words','official_words','official_words+bench_result.count*7'),('ctrl10','&official_ctrl10','(&official_ctrl10+1)'),('window','&fifo_window','(&fifo_window+1)')]:
  if label=='official-words':body+='if bench_result.count > 0\n'
  body+=f'dump binary memory {prefix}/primary-{label}.bin {start} {end}\n'
  if label=='official-words':body+='end\n'
 body+=f'dump binary memory {prefix}/primary-RCC.bin 0x46020C00 0x46020CF0\ndump binary memory {prefix}/primary-PWR.bin 0x46020800 0x46020820\ndump binary memory {prefix}/primary-FLASH-ACR.bin 0x40022000 0x40022004\n'
 h.debug('capture',elf,body)
finally:
 if attempt:
  flash('flash-restore',restore,'M014-H original backup fresh')
  h.debug('restore-readback-reset',oldelf,identity+f'dump binary memory {prefix}/flash-restored-full.bin 0x08000000 0x08200000\nmonitor reset\n')
  assert (P/'flash-restored-full.bin').read_bytes()==(P/'flash-before.bin').read_bytes()
  h.debug('restored-boot',oldelf,identity+'''p tr2_production_stage
p tr2_production_result
p tr2_production_poll_errors
p production_runtime.recovery
p production_runtime.persistent_identity
p production_runtime.application.runtime.configuration_service.recovery_status
p g_serial.tx_busy
p g_serial.tx_fault
p g_serial.uart.ErrorCode
x/3wx 0xE000E100
x/2wx 0xE000ED28
'''+f'dump binary memory {prefix}/persistent-candidate-after.bin production_runtime.candidate (production_runtime.candidate+51914)\n')
  assert (P/'persistent-candidate-before.bin').read_bytes()==(P/'persistent-candidate-after.bin').read_bytes()
  (P/'restoration-verified.json').write_text(json.dumps({'flash_2MiB_byte_equal':True,'flash_sha256':hashlib.sha256((P/'flash-restored-full.bin').read_bytes()).hexdigest(),'persistent_logical_candidate_identical':True,'restore_span_bytes':span,'M014_H_running':True},indent=2)+'\n')
print('Single campaign finished, M014-H restored; inspect raw result')
