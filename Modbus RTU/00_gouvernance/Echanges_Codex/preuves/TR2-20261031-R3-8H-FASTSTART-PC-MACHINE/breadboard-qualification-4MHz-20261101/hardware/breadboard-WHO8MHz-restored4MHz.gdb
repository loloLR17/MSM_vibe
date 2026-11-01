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
if r3_result.stage!=4 || r3_result.run!=3 || r3_result.error || fifo.running || r3_writer.active || r3_command || q.count || r3_clock_fault || (*(unsigned int*)0x46002000 &1) || ((*(unsigned int*)0x46002008>>28)&7)!=1
 quit 8
end
set *(unsigned int*)0x46002008=*(unsigned int*)0x46002008 &~0x70000000
set 'platform.c'::spi.Init.BaudRatePrescaler=0
set {int}(r3_header+352)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+300))
set {int}(r3_header+356)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+301))
set {int}(r3_header+360)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+302))
set {int}(r3_header+364)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+303))
set {int}(r3_header+368)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+304))
set {int}(r3_header+372)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+305))
set {int}(r3_header+376)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+306))
set {int}(r3_header+380)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+307))
set {int}(r3_header+384)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+308))
set {int}(r3_header+388)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+309))
set {int}(r3_header+392)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+310))
set {int}(r3_header+396)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+311))
set {int}(r3_header+400)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+312))
set {int}(r3_header+404)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+313))
set {int}(r3_header+408)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+314))
set {int}(r3_header+412)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+315))
set *(unsigned int*)0x46002008=(*(unsigned int*)0x46002008 &~0x70000000)|0x10000000
set 'platform.c'::spi.Init.BaudRatePrescaler=0x10000000
x/16bx (r3_header+300)
x/16dw (r3_header+352)
p/x *(unsigned int*)0x46002008
p fifo.running
p r3_writer.active
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101/WHO-8MHz-candidate.bin (r3_header+300) (r3_header+416)

detach
quit
