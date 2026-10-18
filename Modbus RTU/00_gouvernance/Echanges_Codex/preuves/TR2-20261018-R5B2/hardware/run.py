from hardware import *
import concurrent.futures,sys,shutil,sqlite3
sys.path.insert(0,str(P.parent));from common import service,E
R=Path('/home/lolo/dev/msm/projects/MSM_vibe-R5B2-physical-resume');mode='live'
B=P.parent/'probe';elf=B/'tr2-r5b-live-to-web-NON-PRODUCTION.elf';image=elf.with_suffix('.bin');ready=P/(mode+'-ready.bin');assert not ready.exists()
code=subprocess.check_output(['git','rev-parse','HEAD'],cwd=R,text=True).strip();assert subprocess.check_output(['git','rev-parse','origin/main'],cwd=R,text=True).strip()==code
assert hashlib.sha256((P/'fram-before.bin').read_bytes()).hexdigest()=='4e7fabde5da1100cf9ae8faf262ea079727856826bb2829c1e99b68ad63b84bd'
assert hashlib.sha256(image.read_bytes()).hexdigest()=='2cf491cc8cfa8da76fe97512b3fb6471a753b3ac89ef9b169bf05745075a27ee'
assert not subprocess.check_output(['git','diff','dc773733','HEAD','--','Modbus RTU/05_Firmware/platform/stm32','Modbus RTU/05_Firmware/src','Modbus RTU/Supervision TR2/src'],cwd=R)
current=Path('/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z/qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf')
debug('before-programming-identity',current,IDENTITY+'p tr2_commission_stage\np tr2_commission_sensor_whoami\n')
flash('live-flash',image,'dc773733a98d78016e1d0f8a341b155e1851e2c7; unchanged source at '+code)
waitline=next(i for i,l in enumerate((R/'Modbus RTU/05_Firmware/platform/stm32/stm32_r4u_runtime.c').read_text().splitlines(),1) if l.startswith(' while(r4_trigger'))
debug('live-image-check',elf,IDENTITY+f'dump binary memory {win(P/"live-image-readback.bin")} 0x08000000 0x08000000+{image.stat().st_size}\n')
assert (P/'live-image-readback.bin').read_bytes()==image.read_bytes()
body=IDENTITY+f'''monitor reset
tbreak stm32_r4u_runtime.c:{waitline}
continue
p r4u_result
if r4u_result.stage != 1 || r4u_result.error
 quit 4
end
break r4u_uart_checkpoint
break r4u_checkpoint
set r4_trigger=0x52345503
continue
p r4u_result
p runtime
if r4u_result.stage != 7 || r4u_result.error || work.error
 quit 5
end
dump binary memory {win(ready)} &r4u_result &r4u_result+1
continue
p r4u_result
p r4_result
p work
p runtime
p metric
p event_time
p r4r_result
p r4s_result
p engine
p live.generation
p store.generation
p store.epoch
p store.effect
p owner.acquisition.fifo
p server.captured
p server.queue_full
p server.transport_errors
p tr2_irq_observer
p g_serial
p r4m_range_count
p r4m_ranges
p r4m_cs_closed
p r4r_first_value
set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
dump binary memory {win(P/'live-fram.bin')} baseline baseline+262144
dump binary memory {win(P/'live-xyz.bin')} r4r_xyz r4r_xyz+4096
if $rr || r4u_result.error || r4u_result.stage != 10 || work.error || work.deadline_missed || work.max_work_us > 1000 || work.max_service_us > 2500 || r4_result.max_gap_us > 5000
 quit 6
end
'''
def command(mode):
 with (P/(mode+'-master.jsonl')).open('wb') as log:z=subprocess.run([str(P.parent/'BenchMaster.exe'),mode],stdout=log,stderr=subprocess.STDOUT,timeout=230)
 z.check_returncode();print('COMMAND',mode,'terminal and handle closed',flush=True)
def initial_check():
 # Service output is raw System.Text.Json (numeric enums); no previous DB observations inferred as current.
 rows=[]
 for line in (P.parent/'initial-service.log').read_text(errors='replace').splitlines():
  if line.startswith('{"Observation"'):rows.append(json.loads(line))
 assert rows and all(x['Observation']['Snapshot'] is not None and x['TransportConnected'] for x in rows), 'Actual B0/B4/B6/B3 route rejected before START'
 assert all(not x['Observation']['Snapshot']['ValuesValid'] for x in rows),'Unexpected preexisting physical measurement'
with concurrent.futures.ThreadPoolExecutor() as pool:
 future=pool.submit(debug,'live-phase',elf,body,650)
 deadline=time.monotonic()+60
 while not ready.exists():
  if future.done():future.result();raise RuntimeError('no ready checkpoint')
  assert time.monotonic()<deadline,'no ready checkpoint';time.sleep(.05)
 initial=pool.submit(service,'initial','COM4',12)
 trace=E/'initial-serial.jsonl';deadline=time.monotonic()+8
 while not(trace.exists() and trace.stat().st_size>0):
  if initial.done():initial.result();raise RuntimeError('no serial read')
  assert time.monotonic()<deadline;time.sleep(.05)
 with (P/'exclusive-check.jsonl').open('wb') as log:z=subprocess.run([str(P.parent/'BenchMaster.exe'),'exclusive'],stdout=log,stderr=subprocess.STDOUT,timeout=20)
 z.check_returncode();initial.result();initial_check();print('Initial real profile accepted; one handle confirmed; no command yet',flush=True)
 command('start')
 service('running','COM4',20,True)
 command('stop')
 service('held','COM4',10)
 future.result()
 assert (P/'live-fram.bin').read_bytes()==(P.parent/'model-final.bin').read_bytes(),'Final FRAM differs from actual R2 host model'
 print('PHYSICAL END, full FRAM exact model; services and command handles closed',flush=True)
