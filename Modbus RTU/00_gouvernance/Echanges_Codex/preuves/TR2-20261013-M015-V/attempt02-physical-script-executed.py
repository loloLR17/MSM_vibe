from pathlib import Path
import subprocess,json,hashlib,time,datetime,importlib.util,base64,sys,shutil
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-M015-V');tmp=Path('/tmp/TR2-mission015-V')
P=Path('/home/lolo/dev/msm/reports')/('TR2-mission015-V-attempt02-'+datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ'));shutil.copytree(tmp,P);Path('/tmp/tr2-m015-v-proof').write_text(str(P))
spec=importlib.util.spec_from_file_location('h','/home/lolo/dev/msm/reports/TR2-mission015-J-20261012T184145Z/hardware.py');h=importlib.util.module_from_spec(spec);spec.loader.exec_module(h);h.P=P
sha=subprocess.check_output(['git','rev-parse','HEAD'],cwd=r).decode().strip();assert sha==subprocess.check_output(['git','rev-parse','origin/main'],cwd=r).decode().strip();assert not subprocess.check_output(['git','status','--porcelain'],cwd=r)
files=subprocess.check_output(['git','diff','--name-only','-z','4faf74f',sha],cwd=r).decode().strip('\0').split('\0')
for f in files:assert subprocess.check_output(['git','show','origin/main:'+f],cwd=r)==(r/f).read_bytes()
(P/'checkpoint-readback.json').write_text(json.dumps({'sha':sha,'files':files,'byte_identical':True},indent=2)+'\n')
image=P/'build-cooperative4/tr2_stm32_p11c.bin';elf=P/'build-cooperative4/tr2_stm32_p11c.elf'
art={f.name:{'size':f.stat().st_size,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()} for f in (P/'build-cooperative4').glob('tr2_stm32_p11c.*') if f.suffix in ('.bin','.elf','.map')}
(P/'artifacts.json').write_text(json.dumps({'source_head':sha,'profile':'CPU160_HSI16_SPI4_PRODUCTION_COOPERATIVE_REFRESH_FIXED','artifacts':art},indent=2)+'\n')
(P/'physical-script-executed.py').write_bytes(Path(__file__).read_bytes())
for f in [image,elf]:assert hashlib.sha256(f.read_bytes()).hexdigest()==art[f.name]['sha256']
old=Path('/home/lolo/dev/msm/reports/TR2-mission014-H-20261012T073729Z/build-production-live');oldelf=old/'tr2_stm32_p11c.elf';prefix=h.win(P).replace('\\','/')
identity='x/3wx 0x0BFA0700\nif *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157\n quit 3\nend\n'
h.debug('identity-before-flash',oldelf,identity+f'dump binary memory {prefix}/persistent-before.bin production_runtime.candidate (production_runtime.candidate+51914)\n')
span=((max(image.stat().st_size,(old/'tr2_stm32_p11c.bin').stat().st_size)+8191)//8192)*8192;restore=P/'restore-pages.bin';restore.write_bytes((P/'flash-before.bin').read_bytes()[:span])
assert restore.read_bytes()[:(old/'tr2_stm32_p11c.bin').stat().st_size]==(old/'tr2_stm32_p11c.bin').read_bytes()
def flash(name,f):
 cmd=[h.CLI,'-c','port=SWD','sn='+h.SN,'freq=8000','-w',h.win(f),'0x08000000','-v','-rst']
 with (P/(name+'-command.json')).open('x') as out:json.dump({'command':cmd,'code_sha':sha,'sha256':hashlib.sha256(f.read_bytes()).hexdigest(),'normal_programming_only':True},out,indent=2)
 z=subprocess.run(cmd,capture_output=True,timeout=50);(P/(name+'.log')).write_bytes(z.stdout+z.stderr);z.check_returncode();assert b'Download verified successfully' in z.stdout
# Extend only approved run name; the task remains two read-only FC03, retry0.
w=P/'hp-tools';w.mkdir()
for name in ['run_commissioning_h.py','commissioning-h.ps1']:
 s=(r/'00_Environnement_developpement/T1000/ssh-hp'/name).read_text().replace("'physical-m015-j-restored'","'physical-m015-j-restored','physical-m015-v-cooperative4-fixed'")
 (w/name).write_text(s)
metadata={'package_name':'M014-H-20261012T073729Z','zip_sha256':'62e5bb63a085366ecfe2671f335f99cd61b022a145d26c5eaf3ca4ec167ca77d','manifest_sha256':'37ad4597af22b75d09f9e1ba89bacc6c3a3cefb2c8a53e344085b0f689ac2068','b0_sha256':'10fcc3a207bbc546752c442ce708995b3183f0e36a27a24a7801f515861bb54c','run_name':'physical-m015-v-cooperative4-fixed'}
(P/'hp-package.json').write_text(json.dumps(metadata,indent=2)+'\n')
SSH='/mnt/c/Windows/System32/OpenSSH/ssh.exe'
body="[DateTimeOffset]::UtcNow.ToString('o')";encoded=base64.b64encode(body.encode('utf-16le')).decode();host_before=datetime.datetime.now(datetime.timezone.utc);z=subprocess.run([SSH,'-T','-o','BatchMode=yes','-o','StrictHostKeyChecking=yes','-o','ConnectTimeout=10','tr2-hp','powershell.exe -NoProfile -NonInteractive -EncodedCommand '+encoded],capture_output=True,timeout=25);host_after=datetime.datetime.now(datetime.timezone.utc);(P/'hp-clock.log').write_bytes(z.stdout+z.stderr);z.check_returncode();remote=datetime.datetime.fromisoformat(z.stdout.decode().strip());offset=remote-host_after
(P/'clock-offset.json').write_text(json.dumps({'host_before':host_before.isoformat(),'host_after':host_after.isoformat(),'hp':remote.isoformat(),'offset_lower_bound_seconds':offset.total_seconds()},indent=2)+'\n')
flashed=False
try:
 flashed=True;flash('flash-cooperative4',image)
 h.debug('readback-reset',elf,identity+f'dump binary memory {prefix}/flash-cooperative4-full.bin 0x08000000 0x08200000\nmonitor reset\n')
 full=(P/'flash-cooperative4-full.bin').read_bytes();assert full[:image.stat().st_size() if False else image.stat().st_size]==image.read_bytes();assert full[span:]==(P/'flash-before.bin').read_bytes()[span:]
 time.sleep(.5)
 gate_body=identity+'''p tr2_production_stage
p tr2_production_result
p production_runtime.recovery
p cooperative
p tr2_fifo_state
p bench_ready
p g_serial.tx_busy
p tr2_irq_observer.rx_callback
x/3wx 0xE000E100
x/2wx 0xE000ED28
x/wx 0x46020CE8
x/6wx 0x46002000
if tr2_production_stage != 2 || tr2_production_result != 0 || production_runtime.recovery.generation != 12 || bench_ready != 1 || cooperative.state != 1 || cooperative.who != 123 || tr2_fifo_fault != 0
 quit 4
end
if g_serial.tx_busy || g_serial.tx_fault || g_serial.uart.ErrorCode || g_serial.event_head != g_serial.event_tail || tr2_irq_observer.rx_callback != 0 || tr2_irq_observer.events_dropped != 0
 quit 5
end
if SystemCoreClock != 160000000 || (*(unsigned int *)0x46020CE8 & 0x18) != 0x10 || (*(unsigned int *)0x46002008 & 0x70000000) != 0x10000000 || (*(unsigned int *)0xE000E104 & 0x20000)==0 || (*(unsigned int *)0xE000E108 & 4)==0
 quit 7
end
'''+f'dump binary memory {prefix}/fram-before-capture.bin official_words (official_words+262144)\ndump binary memory {prefix}/persistent-cumulative-before.bin production_runtime.candidate (production_runtime.candidate+51914)\n'
 h.debug('armed-preflight',elf,gate_body)
 assert hashlib.sha256((P/'fram-before-capture.bin').read_bytes()).hexdigest()=='d732acc557152180fc394097d6a5269ea2f45c1da387c8643f512318adf92fce'
 assert (P/'persistent-cumulative-before.bin').read_bytes()==(P/'persistent-before.bin').read_bytes()
 h.debug('arm-on-next-rx',elf,identity+'if bench_ready != 1 || cooperative.state != 1 || cooperative_arm || cooperative_start_tick\n quit 8\nend\nset var bench_command=0x4D313556\n')
 gate=datetime.datetime.now(datetime.timezone.utc)+offset
 (P/'hp-gate.json').write_text(json.dumps({'utc':gate.isoformat(),'maximum_requests':2,'retries':0,'debuggers_closed':True,'acquisition_armed_on_next_rx':True},indent=2)+'\n')
 z=subprocess.run([sys.executable,str(w/'run_commissioning_h.py'),'Steady','--metadata',str(P/'hp-package.json'),'--evidence',str(P/'hp-cooperative4'),'--gate-utc',gate.isoformat()],capture_output=True,timeout=105);(P/'hp-invocation.log').write_bytes(z.stdout+z.stderr);z.check_returncode()
 time.sleep(1.8)
 body=identity+'''p cooperative
p tr2_fifo_state
p cooperative_fram_state
p cooperative_modbus_count
p cooperative_modbus
p cooperative_burst_count
p cooperative_prefix_xyz
p cooperative_prefix_tick
p cooperative_max_cycles
p cooperative_fram_crc_before
p cooperative_fram_crc_after
p bench_trace_count
p bench_payload_count
p bench_preclose_count
p bench_log_refused
p bench_preclose_overflow
p g_serial
p production_runtime.recovery
p tr2_irq_observer
p tr2_production_poll_errors
p g_serial.uart.ErrorCode
p g_serial.tx_fault
x/2wx 0xE000ED28
if bench_trace_count > 1024 || bench_payload_count > 65536 || bench_preclose_count > 2048 || cooperative.window.received > 36864 || cooperative_burst_count > 320
 quit 6
end
'''
 for label,start,end in [('engine','&cooperative','(&cooperative+1)'),('result','&bench_result','(&bench_result+1)'),('traces','bench_trace','((char *)bench_trace)+bench_trace_count*sizeof(BenchTrace)'),('payload','bench_payload','bench_payload+bench_payload_count'),('preclose','bench_preclose','((char *)bench_preclose)+bench_preclose_count*sizeof(BenchPreclose)'),('words','official_words','official_words+cooperative.window.received*7'),('bursts','cooperative_bursts','((char *)cooperative_bursts)+cooperative_burst_count*sizeof(BurstEvidence)'),('modbus','cooperative_modbus','((char *)cooperative_modbus)+cooperative_modbus_count*sizeof(ModbusEvidence)'),('log-first','&cooperative_log_first_word','(&cooperative_log_first_word+1)'),('start','&cooperative_start_tick','(&cooperative_start_tick+1)'),('end','&cooperative_end_tick','(&cooperative_end_tick+1)'),('max-cycles','&cooperative_max_cycles','(&cooperative_max_cycles+1)'),('prefix-xyz','cooperative_prefix_xyz','(cooperative_prefix_xyz+8)'),('prefix-tick','cooperative_prefix_tick','(cooperative_prefix_tick+8)')]:
  guard={'words':'cooperative.window.received','bursts':'cooperative_burst_count','modbus':'cooperative_modbus_count','traces':'bench_trace_count','payload':'bench_payload_count','preclose':'bench_preclose_count'}.get(label)
  if guard:body+=f'if {guard} > 0\n'
  body+=f'dump binary memory {prefix}/coop4-{label}.bin {start} {end}\n'
  if guard:body+='end\n'
 body+=f'dump binary memory {prefix}/persistent-after-capture.bin production_runtime.candidate (production_runtime.candidate+51914)\ndump binary memory {prefix}/coop4-RCC.bin 0x46020C00 0x46020CF0\n'
 h.debug('capture-after-hp',elf,body)
 # Data are preserved before deciding keep vs qualified restoration.
 h.debug('final-gate',elf,identity+'''if cooperative.state != 4 || tr2_fifo_state != 3 || cooperative_fram_state != 3 || cooperative.result || cooperative.cleanup_result || tr2_production_poll_errors || g_serial.uart.ErrorCode || g_serial.tx_fault || tr2_irq_observer.events_dropped || tr2_irq_observer.error_callback || tr2_irq_observer.tx_fail_safe
 quit 9
end
''')
 assert (P/'persistent-after-capture.bin').read_bytes()==(P/'persistent-before.bin').read_bytes()
 (P/'cumulative-kept.json').write_text(json.dumps({'code_sha':sha,'profile':'SPI4_COOPERATIVE','binary_sha256':hashlib.sha256(image.read_bytes()).hexdigest(),'no_reset_after_capture':True,'M014_H_backup_verified':True,'active_persistent_domain_byte_identical':True,'native_gate_passed':True},indent=2)+'\n')
except Exception:
 if flashed:
  flash('flash-restore-critical',restore)
  h.debug('restore-readback-reset',oldelf,identity+f'dump binary memory {prefix}/flash-restored-full.bin 0x08000000 0x08200000\nmonitor reset\n')
  assert (P/'flash-restored-full.bin').read_bytes()==(P/'flash-before.bin').read_bytes()
  h.debug('restored-boot',oldelf,identity+'p tr2_production_stage\np tr2_production_result\np production_runtime.recovery\nx/3wx 0xE000E100\nx/2wx 0xE000ED28\n')
  (P/'restoration-critical.json').write_text('{"Flash2MiB_byte_identical":true,"M014_H_restored":true}\n')
 raise
print('Cumulative4MHz kept after native success; independent audit and HP full journal still required',P)
