from pathlib import Path
import hashlib,json
import hardware_second as h
P=Path(__file__).parent;f=P/'hardware/current-N8E.bin';image=P/'arm/r3_coex.bin'
h.debug('breadboard-before-init-image-and-idle',P/'arm/r3_coex.elf',h.IDENTITY+'''p r3_result.stage
p r3_result.error
p r3_result.run
p fifo.running
p r3_writer.active
if r3_result.stage!=2 || r3_result.error || r3_result.run || r3_command || fifo.running || r3_writer.active || r3_clock_fault || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end
'''+f'dump binary memory {h.win(f).replace(chr(92),"/")} 0x08000000 {hex(0x08000000+len(image.read_bytes()))}\n')
assert f.read_bytes()==image.read_bytes();print('UID/image exact, WAIT2/run0/error0 and idle; no reset or flash')
