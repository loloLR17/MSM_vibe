set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
info registers pc sp xpsr
p tr2_commission_stage
p tr2_commission_sensor_whoami
p tr2_commission_fram_identity_matches
p tr2_commission_fram_rdid
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r5b2\hardware\flash-before.bin 0x08000000 0x08200000

detach
quit
