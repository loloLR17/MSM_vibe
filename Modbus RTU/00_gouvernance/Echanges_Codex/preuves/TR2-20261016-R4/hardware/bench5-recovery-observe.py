from hardware import *
elf=P.parent/'bench-build/tr2-r4-qualification-NON-PRODUCTION.elf'
record=json.loads((P/'bench5-recovery-trigger-host-time.json').read_text());assert (datetime.datetime.now(datetime.timezone.utc)-datetime.datetime.fromisoformat(record['utc'])).total_seconds()>=15
debug('bench5-recovery-result',elf,IDENTITY+'''p r4_result
p work
p engine.indeterminate
p engine.status
p engine.has_active
p engine.active
p owner.acquisition.fifo.stopped
if r4_result.stage != 10 || r4_result.error != 0 || r4_result.write_bytes != 0 || engine.indeterminate != 1 || engine.status != 9 || r4_result.protected_equal != 1
 quit 9
end
set var fram.timeout_ms=10
set $r4_verify=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r4_verify
if $r4_verify != 0
 quit 8
end
'''+ 'dump binary memory '+win(P/'fram-after-readonly-recovery.bin')+' baseline baseline+262144\n')
assert (P/'fram-after-readonly-recovery.bin').read_bytes()==(P/'fram-after-r2-indeterminate.bin').read_bytes()
print('READONLY_RECOVERY_PASS_WITH_IDENTICAL_FRAM',flush=True)
