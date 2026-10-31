set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result.stage
p r3_result.error
p/x *(unsigned int*)0x46002008
if fifo.running || r3_writer.active || r3_command || (*(unsigned int*)0x46002000 &1) || (*(unsigned int*)0x46002008 &0x70000000)
 quit 8
end
set *(unsigned int*)0x46002008=(*(unsigned int*)0x46002008 &~0x70000000)|0x30000000
set 'platform.c'::spi.Init.BaudRatePrescaler=0x30000000
p/x *(unsigned int*)0x46002008
set $i=0
while $i<16
 set {int}(r3_header+128+4*$i)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+$i))
 set $i=$i+1
end
set *(unsigned int*)0x46002008=*(unsigned int*)0x46002008 &~0x70000000
set 'platform.c'::spi.Init.BaudRatePrescaler=0
p/x *(unsigned int*)0x46002008
while $i<32
 set {int}(r3_header+128+4*$i)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+$i))
 set $i=$i+1
end
x/32bx r3_header
p r3_result.who
p/x *(unsigned int*)0x42021414
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-r3-N8A-new-wires-20261101/hardware/WHO-1MHz-then-8MHz.bin r3_header (r3_header+32)
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-r3-N8A-new-wires-20261101/hardware/WHO-1MHz-then-8MHz-rc.bin (r3_header+128) (r3_header+256)

detach
quit
