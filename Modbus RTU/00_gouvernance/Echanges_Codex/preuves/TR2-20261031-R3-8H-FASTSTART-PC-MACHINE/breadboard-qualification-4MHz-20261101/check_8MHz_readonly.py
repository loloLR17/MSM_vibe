from pathlib import Path
import json,struct
import hardware_second as h
P=Path(__file__).parent;f=P/'WHO-8MHz-candidate.bin'
body=h.IDENTITY+'''p r3_result.stage
p r3_result.error
if r3_result.stage!=4 || r3_result.run!=3 || r3_result.error || fifo.running || r3_writer.active || r3_command || q.count || r3_clock_fault || (*(unsigned int*)0x46002000 &1) || ((*(unsigned int*)0x46002008>>28)&7)!=1
 quit 8
end
set *(unsigned int*)0x46002008=*(unsigned int*)0x46002008 &~0x70000000
set 'platform.c'::spi.Init.BaudRatePrescaler=0
'''
for i in range(16):body+=f'set {{int}}(r3_header+{352+4*i})=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+{300+i}))\n'
body+='''set *(unsigned int*)0x46002008=(*(unsigned int*)0x46002008 &~0x70000000)|0x10000000
set 'platform.c'::spi.Init.BaudRatePrescaler=0x10000000
x/16bx (r3_header+300)
x/16dw (r3_header+352)
p/x *(unsigned int*)0x46002008
p fifo.running
p r3_writer.active
'''
body+=f'dump binary memory {h.win(f).replace(chr(92),"/")} (r3_header+300) (r3_header+416)\n'
h.debug('breadboard-WHO8MHz-restored4MHz',P/'arm/r3_coex.elf',body)
b=f.read_bytes();who=list(b[:16]);rc=list(struct.unpack_from('<16i',b,52));o=dict(WHO=who,return_codes=rc,all_expected=who==[123]*16 and rc==[0]*16,nominal_test_Hz=8000000,restored_Hz=4000000,no_sensor_register_write=True,no_acquisition=True,no_reset=True,no_flash=True,GDB_detached=True);(P/'WHO8MHz.json').write_text(json.dumps(o,indent=2)+'\n');print(json.dumps(o))
