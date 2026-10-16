from hardware import *
assert time.time()-json.loads((P/'before-started.json').read_text())['epoch']>=15
elf=Path('/home/lolo/dev/msm/reports/TR2-r4d/before-build/tr2-r4d-diagnostic-NON-PRODUCTION.elf')
debug('r4d-before-results',elf,IDENTITY+'''set print elements 600
p r4d
p *r4d_exercises@r4d.count
p *r4d_transfers@r4d.trace_count
p work
p owner.acquisition.fifo
p bench_log_refused
p bench_preclose_overflow
if r4d.stage != 100 || r4d.error != 0 || owner.acquisition.fifo.running
 quit 4
end
set $r=(unsigned int)stm32_fram_storage_read(&fram,0,r4d_backup,262144)
p $r
if $r != 0
 quit 5
end
'''+f'dump binary memory {win(P/"fram-after-before.bin")} r4d_backup r4d_backup+262144\n',timeout=60)
assert (P/'fram-after-before.bin').read_bytes()==(P/'fram-before.bin').read_bytes();print('BEFORE FULL FRAM UNCHANGED')
