set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if fifo.running || r3_writer.active || r3_command || r3_clock_fault || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end
set {int}(r3_header+352)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+300))
set {int}(r3_header+356)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+301))
set {int}(r3_header+360)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+302))
set {int}(r3_header+364)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+303))
set {int}(r3_header+368)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+304))
set {int}(r3_header+372)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+305))
set {int}(r3_header+376)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+306))
set {int}(r3_header+380)=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+307))
x/8bx (r3_header+300)
x/8dw (r3_header+352)
if r3_header[300]==0x7b && r3_header[301]==0x7b && r3_header[302]==0x7b && r3_header[303]==0x7b && r3_header[304]==0x7b && r3_header[305]==0x7b && r3_header[306]==0x7b && r3_header[307]==0x7b
set {int}(r3_header+384)=(int)iis3dwb_read_reg(&fifo.ctx,0x10,(unsigned char*)(r3_header+310),1)
set {int}(r3_header+388)=(int)iis3dwb_read_reg(&fifo.ctx,0x0a,(unsigned char*)(r3_header+311),1)
set {int}(r3_header+392)=(int)iis3dwb_read_reg(&fifo.ctx,0x19,(unsigned char*)(r3_header+312),1)
x/3bx (r3_header+310)
x/3dw (r3_header+384)
end
p r3_result.stage
p r3_result.error
p r3_result.run
p fifo.running
p r3_writer.active
p r3_card.writes
p/x *(unsigned int*)0x42021414
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-quick-20261101/who-and-controls.bin (r3_header+300) (r3_header+396)

detach
quit
