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
if fifo.running || r3_writer.active || r3_command || (*(unsigned int*)0x46002000 &1) || ((*(unsigned int*)0x46002008>>28)&7)!=1
 quit 8
end
set *(unsigned int*)0x46002008=(*(unsigned int*)0x46002008 &~0x70000000)|0x30000000
set 'platform.c'::spi.Init.BaudRatePrescaler=0x30000000
set $a=(int)iis3dwb_read_reg(&fifo.ctx,0x10,(unsigned char*)(r3_header+300),1)
set $b=(int)iis3dwb_read_reg(&fifo.ctx,0x0a,(unsigned char*)(r3_header+301),1)
set $c=(int)iis3dwb_read_reg(&fifo.ctx,0x19,(unsigned char*)(r3_header+302),1)
x/3bx (r3_header+300)
if $a || $b || $c || (r3_header[300]&0xe0) || (r3_header[301]&7) || (r3_header[302]&0x20)
 quit 9
end
set {unsigned char}(r3_header+320)=0x80
set *(unsigned int*)0x46002008=(*(unsigned int*)0x46002008 &~0x70000000)|0x10000000
set 'platform.c'::spi.Init.BaudRatePrescaler=0x10000000
set {int}(r3_header+352)=(int)iis3dwb_write_reg(&fifo.ctx,0x0a,(unsigned char*)(r3_header+320),1)
set {int}(r3_header+356)=(int)iis3dwb_read_reg(&fifo.ctx,0x0a,(unsigned char*)(r3_header+321),1)
set *(unsigned int*)0x46002008=(*(unsigned int*)0x46002008 &~0x70000000)|0x30000000
set 'platform.c'::spi.Init.BaudRatePrescaler=0x30000000
set {int}(r3_header+360)=(int)iis3dwb_read_reg(&fifo.ctx,0x0a,(unsigned char*)(r3_header+322),1)
set {int}(r3_header+364)=(int)iis3dwb_write_reg(&fifo.ctx,0x0a,(unsigned char*)(r3_header+320),1)
set {int}(r3_header+368)=(int)iis3dwb_read_reg(&fifo.ctx,0x0a,(unsigned char*)(r3_header+323),1)
set *(unsigned int*)0x46002008=(*(unsigned int*)0x46002008 &~0x70000000)|0x10000000
set 'platform.c'::spi.Init.BaudRatePrescaler=0x10000000
set {int}(r3_header+372)=(int)iis3dwb_read_reg(&fifo.ctx,0x0a,(unsigned char*)(r3_header+324),1)
set *(unsigned int*)0x46002008=(*(unsigned int*)0x46002008 &~0x70000000)|0x30000000
set 'platform.c'::spi.Init.BaudRatePrescaler=0x30000000
set {int}(r3_header+376)=(int)iis3dwb_write_reg(&fifo.ctx,0x0a,(unsigned char*)(r3_header+301),1)
set {int}(r3_header+380)=(int)iis3dwb_read_reg(&fifo.ctx,0x0a,(unsigned char*)(r3_header+325),1)
set *(unsigned int*)0x46002008=(*(unsigned int*)0x46002008 &~0x70000000)|0x10000000
set 'platform.c'::spi.Init.BaudRatePrescaler=0x10000000
x/6bx (r3_header+320)
x/8dw (r3_header+352)
p/x *(unsigned int*)0x46002008
p r3_result.error
p fifo.running
p r3_writer.active
p/x *(unsigned int*)0x42021414
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-r3-N8E-duplex4-20261101/hardware/config-bit-crosscheck.bin (r3_header+300) (r3_header+384)

detach
quit
