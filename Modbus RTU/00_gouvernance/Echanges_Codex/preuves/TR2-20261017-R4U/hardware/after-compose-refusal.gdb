set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4u_result
p r4_result
p work
p owner.acquisition.fifo.running
p owner.acquisition.metrology
p owner.acquisition.engine
p engine.has_active
p g_serial.uart.Instance
if owner.acquisition.fifo.running || r4_result.write_bytes || engine.has_active
 quit 4
end
set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4u\hardware\refusal-fram.bin baseline baseline+262144

detach
quit
