from hardware import *
elf=P.parent/'bench-build/tr2-r4-qualification-NON-PRODUCTION.elf'
debug('bench3-stopped-full-fram-control',elf,IDENTITY+'''p r4_result
p owner.acquisition.fifo.stopped
if owner.acquisition.fifo.stopped != 1 || r4_result.write_bytes != 0
 quit 7
end
set var fram.timeout_ms=10
set $r4_control=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r4_control
if $r4_control != 0
 quit 8
end
'''+ 'dump binary memory '+win(P/'bench3-fram-after-readonly.bin')+' baseline baseline+262144\n')
assert (P/'bench3-fram-after-readonly.bin').read_bytes()==(P/'fram-before.bin').read_bytes()
print('FRAM_ALL_262144_BYTES_UNCHANGED',flush=True)
debug('bench3-r2-reset',elf,IDENTITY+'monitor reset\n')
time.sleep(2)
debug('bench3-r2-trigger',elf,IDENTITY+'''p r4_result
if r4_result.stage != 1 || r4_result.error != 0
 quit 5
end
set var r4_trigger=0x52340002
''')
(P/'r2-trigger-host-time.json').write_text(json.dumps(dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),not_before_observe_seconds=210))+'\n')
