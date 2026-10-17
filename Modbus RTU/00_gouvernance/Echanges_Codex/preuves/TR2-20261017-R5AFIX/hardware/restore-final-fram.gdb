set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
frame function CommissioningReadOnly
p tr2_commission_stage
set $r=(unsigned int)stm32_fram_storage_read(&storage,0,tr2_commission_fram_backup,262144)
p $r
if $r != 0
 quit 5
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r5afix\hardware\fram-after-restoration.bin tr2_commission_fram_backup tr2_commission_fram_backup+262144
frame 0

detach
quit
