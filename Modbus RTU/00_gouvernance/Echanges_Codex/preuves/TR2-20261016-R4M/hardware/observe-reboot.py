from hardware import *
assert time.time()-json.loads((P/'r4m-reboot-started.json').read_text())['epoch']>=15
elf=Path('/home/lolo/dev/msm/reports/TR2-r4m/bench-build/tr2-r4m-qualification-NON-PRODUCTION.elf')
debug('r4m-reboot-observation5',elf,IDENTITY+'''p r4_result
p work
p r4m_guard
p engine
p store.epoch
p store.effect
p configuration.active
p owner.acquisition.fifo
if r4_result.stage != 10 || r4_result.error != 0 || r4_result.write_bytes || owner.acquisition.fifo.running
 quit 4
end
set $r=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r
if $r != 0
 quit 5
end
'''+f'dump binary memory {win(P/"fram-after-reboot.bin")} baseline baseline+262144\n',timeout=60)
assert (P/'fram-after-reboot.bin').read_bytes()==(P/'fram-after-resume.bin').read_bytes()
(P/'reboot-summary.json').write_text(json.dumps(dict(result='PASS',zero_writes=True,full_fram_unchanged=True,no_auto_start=True,utc=datetime.datetime.now(datetime.timezone.utc).isoformat()),indent=2)+'\n')
