from hardware import *
import concurrent.futures,sys
R=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4V-rtu-latency');mode=sys.argv[1];assert mode in ['baseline','paced'];pace='0' if mode=='baseline' else '250'
B=P.parent/('probe-'+pace);elf=B/'tr2-r4v-latency-NON-PRODUCTION.elf';image=elf.with_suffix('.bin');ready=P/(mode+'-ready.bin');assert not ready.exists()
code=subprocess.check_output(['git','rev-parse','HEAD'],cwd=R,text=True).strip();assert subprocess.check_output(['git','rev-parse','origin/main'],cwd=R,text=True).strip()==code
# Fresh initial preflight must be available; phase2 additionally requires exact
# phase1 physical vs independently generated metadata. Never restore a FRAM.
assert hashlib.sha256((P/'fram-before.bin').read_bytes()).hexdigest()=='95dab7909dbbab0a85abf1a3d9f8292bafb48dc98de73f026ce2786251b9af41'
if mode=='paced':assert (P/'baseline-fram.bin').read_bytes()==(P.parent/'model-baseline.bin').read_bytes()
# Normal programming only, exactly identified artifact and source state.
meta=json.loads((P.parent/'images.json').read_text())[pace];assert hashlib.sha256(image.read_bytes()).hexdigest()==meta['bin_sha256']
if mode=='baseline':
 current=Path('/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z/qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf')
 debug('baseline-before-programming-identity',current,IDENTITY+'p tr2_commission_stage\np tr2_commission_sensor_whoami\n')
else:
 current=P.parent/'probe-0/tr2-r4v-latency-NON-PRODUCTION.elf'
 debug('paced-before-programming-stopped',current,IDENTITY+'''p r4u_result
p owner.acquisition.fifo.stopped
if owner.acquisition.fifo.running || !owner.acquisition.fifo.stopped || owner.acquisition.fifo.stop_error || engine.has_active || store.generation != 54 || live.generation != 67
 quit 4
end
''')
flash(mode+'-flash',image,code)
waitline=next(i for i,l in enumerate((R/'Modbus RTU/05_Firmware/platform/stm32/stm32_r4u_runtime.c').read_text().splitlines(),1) if l.startswith(' while(r4_trigger'))
debug(mode+'-image-check',elf,IDENTITY+f'dump binary memory {win(P/(mode+"-image-readback.bin"))} 0x08000000 0x08000000+{image.stat().st_size}\n')
assert (P/(mode+'-image-readback.bin')).read_bytes()==image.read_bytes()
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
p r4v_result
p runtime
if r4u_result.stage != 7 || r4u_result.error || work.error
 quit 5
end
dump binary memory {win(ready)} &r4u_result &r4u_result+1
continue
p r4u_result
p r4v_result
p r4v_events
p r4v_spans
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
dump binary memory {win(P/(mode+'-fram.bin'))} baseline baseline+262144
dump binary memory {win(P/(mode+'-xyz.bin'))} r4r_xyz r4r_xyz+4096
if $rr || r4u_result.error || r4u_result.stage != 10 || r4v_result.dropped || work.error || work.deadline_missed || work.max_work_us > 1000 || work.max_service_us > 2500 || r4_result.max_gap_us > 5000
 quit 6
end
'''
with concurrent.futures.ThreadPoolExecutor() as pool:
 future=pool.submit(debug,mode+'-phase',elf,body,650)
 deadline=time.monotonic()+50
 while not ready.exists():
  if future.done():future.result();raise RuntimeError('no ready checkpoint')
  assert time.monotonic()<deadline,'no ready checkpoint within50s'
  time.sleep(.05)
 with (P/(mode+'-master.jsonl')).open('wb') as log:
  master=subprocess.run([str(P.parent/'BenchMaster.exe'),'live',mode],stdout=log,stderr=subprocess.STDOUT,timeout=480)
 print('MASTER',mode,master.returncode,flush=True);future.result();master.check_returncode()
 expected=P.parent/('model-baseline.bin' if mode=='baseline' else 'model-paced.bin');assert (P/(mode+'-fram.bin')).read_bytes()==expected.read_bytes()
 print('PHASE',mode,'PASS; full FRAM exact model',flush=True)
