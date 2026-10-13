set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p bench_ready
p tr2_fifo_state
p tr2_fifo_fault
p tr2_fifo_fram_status
p bench_core_hz
p SystemCoreClock
x/10wx 0x46020C00
x/wx 0x46020CE8
x/wx 0x4602080C
x/wx 0x40022000
x/6wx 0x46002000
x/2wx 0xE000ED28
if bench_ready != 1 || tr2_fifo_state != 1 || tr2_fifo_fram_status != 1 || SystemCoreClock != 160000000 || bench_core_hz != 160000000
 quit 4
end
if (*(unsigned int *)0x46020CE8 & 0x18) != 0x10 || (*(unsigned int *)0x40022000 & 15) != 4 || (*(unsigned int *)0x46002008 & 0x70000000) != 0x10000000
 quit 7
end
dump binary memory //wsl.localhost/Debian/tmp/TR2-mission015-U/qualification/fram-before-capture.bin tr2_fifo_fram_backup (tr2_fifo_fram_backup+262144)

detach
quit
