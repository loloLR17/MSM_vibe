from hardware import *
assert time.time()-json.loads((P/'readonly-started.json').read_text())['epoch']>=15
elf=Path('/home/lolo/dev/msm/reports/TR2-r4p/resume-build/tr2-r4p-coexistence-NON-PRODUCTION.elf')
debug('r4p-readonly-results7',elf,IDENTITY+'''p r4_result
p work
p r4m_guard
p r4m_protected_bytes
p r4m_protected_checks
p engine
p store.epoch
p store.effect
p owner.acquisition.fifo
if r4_result.stage != 10 || r4_result.error || r4_result.write_bytes || work.error || work.deadline_missed || work.max_work_us > 1000 || work.max_service_us > 2500 || r4m_protected_bytes != 139264 || owner.acquisition.fifo.running || engine.indeterminate || engine.has_active || r4m_guard.phase != 0
 quit 4
end
set $r=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r
if $r != 0
 quit 5
end
'''+f'dump binary memory {win(P/"fram-after-readonly.bin")} baseline baseline+262144\n',timeout=60)
assert (P/'fram-after-readonly.bin').read_bytes()==(P/'fram-after-physical.bin').read_bytes()
(P/'readonly-qualified.json').write_text(json.dumps(dict(result='PASS',protected_bytes_compared=139264,full_fram_unchanged=True,fram_writes=0,epoch=2,run_floor=2,utc=datetime.datetime.now(datetime.timezone.utc).isoformat()),indent=2)+'\n');print('READONLY QUALIFIED; SAME BRIDGE, NO WRITES')
