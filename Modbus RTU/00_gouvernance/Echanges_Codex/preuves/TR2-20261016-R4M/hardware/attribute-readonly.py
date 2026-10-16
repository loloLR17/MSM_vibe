from hardware import *
elf=Path('/home/lolo/dev/msm/reports/TR2-r4m/bench-build/tr2-r4m-qualification-NON-PRODUCTION.elf')
debug('r4m-readonly-deadline-attribution',elf,IDENTITY+'''p owner.acquisition.fifo.running
if owner.acquisition.fifo.running
 quit 4
end
set r4_result.ownership=0
set work.error=0
set work.seen=0
set work.depth=0
set work.deadline_missed=0
set work.max_work_us=0
break failed
set $recover=(unsigned int)live_command_store_recover(&store)
bt
p work
p r4_result.write_bytes
p r4m_guard
''',timeout=60)
