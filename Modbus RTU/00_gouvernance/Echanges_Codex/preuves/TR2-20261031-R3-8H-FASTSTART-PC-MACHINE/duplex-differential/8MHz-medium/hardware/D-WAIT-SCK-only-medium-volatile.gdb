set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61236
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result.stage
if r3_result.stage!=2 || r3_result.run || r3_result.error || fifo.running || r3_writer.active
 quit 8
end
p/x *(unsigned int*)0x42020808
if ((*(unsigned int*)0x42020808 >>20)&3)!=2
 quit 9
end
set *(unsigned int*)0x42020808 = (*(unsigned int*)0x42020808 & ~0x00300000) | 0x00100000
p/x *(unsigned int*)0x42020808
p/x *(unsigned int*)0x46002008

detach
quit
