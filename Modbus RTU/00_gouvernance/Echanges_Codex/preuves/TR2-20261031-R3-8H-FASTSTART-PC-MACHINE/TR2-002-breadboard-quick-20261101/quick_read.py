from pathlib import Path
import importlib.util,hashlib,json,struct,subprocess,datetime
P=Path(__file__).parent;OLD=Path('/home/lolo/dev/msm/reports/TR2-r3-N8E-duplex4-20261101');a=json.loads((OLD/'artifact.json').read_text());elf=OLD/'arm/r3_coex.elf';image=OLD/'arm/r3_coex.bin'
assert hashlib.sha256(image.read_bytes()).hexdigest()==a['images'][0]['sha256']
spec=importlib.util.spec_from_file_location('hw',OLD/'hardware_second.py');h=importlib.util.module_from_spec(spec);spec.loader.exec_module(h);h.P=P/'hardware'
(P/'STLINK_LIST.log').write_bytes(subprocess.check_output([h.CLI,'-l','stlink']))
f=P/'hardware/flash-prefix.bin'
h.debug('identity-image-idle',elf,h.IDENTITY+'''p r3_result.stage
p r3_result.error
p r3_result.run
p fifo.running
p r3_writer.active
p r3_clock_fault
p/x *(unsigned int*)0x46002008
p/x *(unsigned int*)0x4600200c
if fifo.running || r3_writer.active || r3_command || r3_clock_fault || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end
'''+f'dump binary memory {h.win(f).replace(chr(92),"/")} 0x08000000 {hex(0x08000000+len(image.read_bytes()))}\n')
assert f.read_bytes()==image.read_bytes(),'Firmware differs: sensor calls refused'
body=h.IDENTITY+'''if fifo.running || r3_writer.active || r3_command || r3_clock_fault || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end
'''
for i in range(8):body+=f'set {{int}}(r3_header+{352+4*i})=(int)iis3dwb_device_id_get(&fifo.ctx,(unsigned char*)(r3_header+{300+i}))\n'
body+='x/8bx (r3_header+300)\nx/8dw (r3_header+352)\n'
body+='''if r3_header[300]==0x7b && r3_header[301]==0x7b && r3_header[302]==0x7b && r3_header[303]==0x7b && r3_header[304]==0x7b && r3_header[305]==0x7b && r3_header[306]==0x7b && r3_header[307]==0x7b
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
'''
raw=P/'who-and-controls.bin';body+=f'dump binary memory {h.win(raw).replace(chr(92),"/")} (r3_header+300) (r3_header+396)\n'
h.debug('WHO-eight-readonly-current-4MHz',elf,body)
b=raw.read_bytes();who=list(b[:8]);rc=list(struct.unpack_from('<8i',b,52));controls=list(b[10:13]) if who==[123]*8 else None
out=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),UID='001B0046/38425018/20333157',SN=h.SN,code_sha=a['code_sha'],BIN_sha256=a['images'][0]['sha256'],entire_BIN_identical=True,SPI_nominal_Hz=4000000,WHO_reads=who,WHO_return_codes=rc,WHO_all_expected=who==[123]*8 and rc==[0]*8,controls_read_only=controls,control_return_codes=list(struct.unpack_from('<3i',b,84)) if controls is not None else None,no_flash=True,no_reset=True,no_calibration_trigger=True,no_acquisition_START=True,no_SD_access=True,no_sensor_register_write=True,GDB_detached=True,user_reports='All Dupont wires through breadboard; TR2-002 ST-LINK connected; TR2-001 disconnected',sensor_module_identity_not_yet_confirmed=True)
(P/'RESULT.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out,indent=2))
