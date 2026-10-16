set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4q_result
p r4_result
p work
p engine
p store.epoch
p store.effect
p owner.acquisition.fifo
p qserver.captured
p qserver.queue_full
p qserver.owner_calls
p qserver.transport_errors
p qserver.time_fault
p qserver.images[qserver.published].b3
if r4q_result.stage != 10 || r4q_result.error || r4_result.write_bytes || owner.acquisition.fifo.running
 quit 4
end
set $r=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r
if $r != 0
 quit 5
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4q\hardware\fram-after-probe.bin baseline baseline+262144

detach
quit
