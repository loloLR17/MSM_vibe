from hardware import *
import concurrent.futures,sys
mode=sys.argv[1];assert mode in ('ro','live')
elf=P.parent/'probe-build-01/tr2-r4u-live-runtime-NON-PRODUCTION.elf'
stage=3 if mode=='ro' else 5;token='0x52345502' if mode=='ro' else '0x52345503'
ready=P/(mode+'-ready.bin');assert not ready.exists()
body=IDENTITY+f'''p r4u_result
if r4u_result.stage != {stage} || r4u_result.error || work.error || work.deadline_missed || work.max_work_us > 1000 || work.max_service_us > 2500 || r4_result.max_gap_us > 5000
 quit 4
end
break r4u_uart_checkpoint
break r4u_checkpoint
set r4_trigger={token}
continue
p r4u_result
p g_serial
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
p r4r_first_value
set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
dump binary memory {win(P/(mode+'-fram.bin'))} baseline baseline+262144
dump binary memory {win(P/(mode+'-first-xyz.bin'))} r4r_xyz r4r_xyz+4096
if $rr || r4u_result.error || work.error || work.deadline_missed || work.max_work_us > 1000 || work.max_service_us > 2500 || r4_result.max_gap_us > 5000
 quit 5
end
'''
with concurrent.futures.ThreadPoolExecutor() as pool:
 future=pool.submit(debug,mode+'-phase',elf,body,660)
 deadline=time.monotonic()+50
 while not ready.exists():
  if future.done():future.result();raise RuntimeError('No ready checkpoint')
  assert time.monotonic()<deadline,'ready deadline'
  time.sleep(.05)
 with (P/(mode+'-master.jsonl')).open('wb') as log:
  master=subprocess.run([str(P.parent/'BenchMaster.exe'),mode],stdout=log,stderr=subprocess.STDOUT,timeout=480)
 print('MASTER',mode,master.returncode,flush=True)
 (P/(mode+'-master-return.json')).write_text(json.dumps(dict(mode=mode,returncode=master.returncode,utc=datetime.datetime.now(datetime.timezone.utc).isoformat()),indent=2)+'\n')
 future.result()
 if mode=='ro':assert (P/'ro-fram.bin').read_bytes()==(P/'fram-before.bin').read_bytes()
 master.check_returncode()
