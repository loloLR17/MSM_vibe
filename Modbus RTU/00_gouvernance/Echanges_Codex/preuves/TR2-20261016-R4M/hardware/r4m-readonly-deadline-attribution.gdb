set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p owner.acquisition.fifo.running
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

detach
quit
