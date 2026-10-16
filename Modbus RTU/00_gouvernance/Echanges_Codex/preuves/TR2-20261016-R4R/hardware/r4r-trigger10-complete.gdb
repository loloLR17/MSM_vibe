set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4r_result
if r4r_result.stage != 1 || r4r_result.error != 0
 quit 5
end
break r4r_checkpoint
set r4_trigger=0x5234000A
continue
p r4r_result
p diag_fifo
p r4_result
p work
printf "CHECKPOINT_STAGE=%u ERROR=%u\n",r4r_result.stage,r4r_result.error
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4r\hardware\raw02.bin r4r_raw r4r_raw+7*r4r_result.raw_words
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4r\hardware\brackets02.bin r4r_brackets &r4r_brackets[r4r_result.brackets]
set $read=(int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $read
if $read != 0
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4r\hardware\fram-after-calibration.bin baseline baseline+262144

detach
quit
