from pathlib import Path
import json,struct
import hardware_second as h
P=Path(__file__).parent;f=P/'hardware/config-bit-crosscheck.bin'
body=h.IDENTITY+'''p r3_result.stage
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
'''
body+=f'dump binary memory {h.win(f).replace(chr(92),"/")} (r3_header+300) (r3_header+384)\n'
h.debug('N8E-config-bit-1-and-4MHz-bypass-only',P/'arm/r3_coex.elf',body)
b=f.read_bytes();rc=list(struct.unpack_from('<8i',b,52));vals=list(b[20:26]);out=dict(initial_controls_at_1MHz=list(b[:3]),written_value=vals[0],write4_read4=vals[1],write4_read1=vals[2],write1_read1=vals[3],write1_read4=vals[4],restored_CTRL4_read1=vals[5],return_codes=rc,restored_SPI3_Hz=4000000,no_FIFO_or_ODR_or_TS_start=True,no_calibration_retry=True)
(P/'CONFIG_BIT_CROSSCHECK.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out))
