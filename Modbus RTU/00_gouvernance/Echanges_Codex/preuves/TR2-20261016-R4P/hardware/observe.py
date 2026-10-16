from hardware import *
started=json.loads((P/'r4p-started.json').read_text());elapsed=time.time()-started['epoch'];assert elapsed>=300,elapsed
elf=Path('/home/lolo/dev/msm/reports/TR2-r4p/bench-build/tr2-r4p-coexistence-NON-PRODUCTION.elf')
body=IDENTITY+'''set print elements 300
p r4_result
p work
p r4m_guard
p r4m_range_count
p r4m_op_count
p r4m_old_rejected
p r4m_protected_checks
p r4m_cs_closed
p r4m_fifo_max_all
p r4m_xyz_all
p r4m_ts_all
p r4m_stop_max_us
p r4m_stop_end_us
p r4m_fault_seen_us
p r4m_fault_reaction_us
p bench_log_refused
p bench_preclose_overflow
p engine
p store.epoch
p store.effect
p configuration.active
p owner.acquisition.fifo
if r4m_range_count > 0
p *r4m_ranges@r4m_range_count
end
if r4m_op_count > 0
p *r4m_operations@r4m_op_count
end
'''
lines=[]
for line in body.splitlines():
 if line.startswith('p '):lines.append('printf "R4M_LABEL '+line[2:]+'\\n"')
 lines.append(line)
debug('r4p-observation4',elf,'\n'.join(lines),timeout=60)
