set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result
if r3_result.stage!=4 || r3_result.error || r3_result.run!=3 || fifo.running || r3_writer.active || r3_writer.uncertain || r3_writer.needs_power_cycle || q.count || r3_card.failed || r3_command || r3_clock_fault || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end
set $a=(int)iis3dwb_read_reg(&fifo.ctx,0x10,(unsigned char*)(r3_header+300),1)
set $b=(int)iis3dwb_read_reg(&fifo.ctx,0x0a,(unsigned char*)(r3_header+301),1)
set $c=(int)iis3dwb_read_reg(&fifo.ctx,0x19,(unsigned char*)(r3_header+302),1)
x/3bx (r3_header+300)
if $a || $b || $c || (r3_header[300]&0xe0) || (r3_header[301]&7) || (r3_header[302]&0x20)
 quit 9
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/hardware/previous-N8E-readback.bin 0x08000000 0x800cf2c

detach
quit
