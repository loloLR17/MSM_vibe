set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61236
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result.stage
p r3_result.error
p fifo.error
p fifo.stop_error
p fifo.parser
p/x *(unsigned int*)0x46002000
if fifo.running || r3_writer.active || !(*(unsigned int*)0x42021414 &0x1000) || (*(unsigned int*)0x46002000 &1)
 quit 8
end
set *(unsigned int*)0x46002008 = *(unsigned int*)0x46002008 &~0x70000000
set 'platform.c'::spi.Init.BaudRatePrescaler=0
p/x *(unsigned int*)0x46002008
p/x *(unsigned int*)0x42020808
if ((*(unsigned int*)0x42020808>>20)&3)!=2
 quit 9
end
p r3_writer.active
p r3_card.failed
p r3_clock_fault
p/x *(unsigned int*)0x42021414

detach
quit
