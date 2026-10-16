from hardware import *
assert time.time()-json.loads((P/'started.json').read_text())['epoch']>=25
elf=P.parent/'probe-build/tr2-r4q-readonly-rtu-NON-PRODUCTION.elf'
debug('r4q-terminal-observation',elf,IDENTITY+'''p r4q_result
p r4_result
p work
p engine
p store.epoch
p store.effect
p owner.acquisition.fifo
p qserver.captured
p qserver.queue_full
p qserver.owner_calls
p qserver.transport_errors
p qserver.time_fault
p qserver.images[qserver.published].b3
if r4q_result.stage != 10 || r4q_result.error || r4_result.write_bytes || owner.acquisition.fifo.running
 quit 4
end
set $r=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r
if $r != 0
 quit 5
end
'''+f'dump binary memory {win(P/"fram-after-probe.bin")} baseline baseline+262144\n',timeout=60)
a=(P/'fram-before.bin').read_bytes();b=(P/'fram-after-probe.bin').read_bytes();assert a==b
(P/'physical-summary.json').write_text(json.dumps(dict(result='PASS',entire_fram_unchanged=True,bytes=262144,sha256=hashlib.sha256(b).hexdigest(),scope='MCU FRAM readonly + RAM RTU loopback, sensor stopped, zero FRAM command admission; NOT electrical RTU or physical B3 vibration',utc=datetime.datetime.now(datetime.timezone.utc).isoformat()),indent=2)+'\n')
