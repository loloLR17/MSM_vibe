set pagination off
set confirm off
set print elements 0
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p ff_result
p hsd1
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-SDMMC1-FIRSTFAULT-20261014\RUN-A-result.bin &ff_result ((unsigned char *)&ff_result)+sizeof(ff_result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-SDMMC1-FIRSTFAULT-20261014\RUN-A-data512.bin ff_data512 ff_data512+512
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-SDMMC1-FIRSTFAULT-20261014\RUN-A-data8192.bin ff_data8192 ff_data8192+8192

if ff_result.stage != 99 && ff_result.stage != 100
 printf "OPERATOR_TIMEOUT_HALTED: no detach/resume, reset or retry\n"
 disconnect
 quit 6
end

detach
quit
