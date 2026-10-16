set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if owner.acquisition.fifo.running != 0
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

detach
quit
