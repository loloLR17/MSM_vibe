from hardware import *
elf=P.parent/'bench-build/tr2-r4-qualification-NON-PRODUCTION.elf'
debug('bench4-readonly-deadline-attribution',elf,IDENTITY+'''if owner.acquisition.fifo.running != 0
 quit 7
end
set $r4_reinit=(int)tr2_cooperative_init(&work,0,work.now_us,work.service,work.safety_stop,32,1000,2500)
p $r4_reinit
break cooperative_work.c:failed
python
try:
 gdb.execute("call live_configuration_recover(&configuration)")
except gdb.error as e:
 print("READONLY_DIAGNOSTIC_STOP",str(e))
end
bt
p work
''',timeout=45)
