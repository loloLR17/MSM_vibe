set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
set print elements 600
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
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4d\hardware\fram-after-corrected.bin r4d_backup r4d_backup+262144

detach
quit
