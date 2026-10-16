set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4r_result
p r4_result
p work
p owner.acquisition.fifo
p engine.status
p store.epoch
p store.effect
p/x *(unsigned int*)0xE000E100
p/x *(unsigned int*)0xE000E104
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4r\hardware\raw.bin r4r_raw r4r_raw+7*r4r_result.raw_words
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4r\hardware\brackets.bin r4r_brackets &r4r_brackets[r4r_result.brackets]
set $read=(int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $read
if $read != 0
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4r\hardware\fram-after-probe.bin baseline baseline+262144

detach
quit
