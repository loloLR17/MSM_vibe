set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61236
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result.stage
p/x *(unsigned int*)0x46002000
p/x *(unsigned int*)0x46002008
if r3_result.stage!=2 || r3_result.run || r3_result.error || fifo.running || r3_writer.active || (*(unsigned int*)0x46002000 &1)
 quit 8
end
if (*(unsigned int*)0x46002008 &0x70000000)!=0
 quit 9
end
set *(unsigned int*)0x46002008 = (*(unsigned int*)0x46002008 &~0x70000000) | 0x10000000
set 'platform.c'::spi.Init.BaudRatePrescaler=0x10000000
p/x *(unsigned int*)0x46002008
p/x *(unsigned int*)0x42020808

detach
quit
