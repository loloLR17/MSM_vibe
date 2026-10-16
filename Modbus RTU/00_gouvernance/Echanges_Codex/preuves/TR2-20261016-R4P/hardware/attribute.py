from hardware import *
elf=Path('/home/lolo/dev/msm/reports/TR2-r4p/bench-build/tr2-r4p-coexistence-NON-PRODUCTION.elf')
try:
 debug('r4p-protected-skip-attribution',elf,IDENTITY+'''p owner.acquisition.fifo.running
if owner.acquisition.fifo.running
 quit 4
end
set r4_result.ownership=0
set work.error=0
set work.depth=0
set work.seen=0
set work.deadline_missed=0
set work.max_work_us=0
break failed
set $r=(int)compare_protected()
''',timeout=60,post_commands=['bt','p work','frame 2','info locals','detach','quit'])
except subprocess.CalledProcessError:
 print('Expected debugger call interrupted at failure breakpoint; inspect preserved stack',flush=True)
