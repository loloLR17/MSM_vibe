from pathlib import Path
import json,hashlib,subprocess
import hardware_second as h
P=Path(__file__).parent;E=Path('/home/lolo/dev/msm/reports/TR2-002-breadboard-validation-20261101');a=json.loads((P/'artifact.json').read_text());old_elf=E/'arm/r3_coex.elf';old_image=E/'arm/r3_coex.bin'
assert json.loads((E/'remote-readback.json').read_text())['all_identical']
for r in a['images']:
 b=(P/'arm'/r['file']).read_bytes();assert len(b)==r['bytes'] and hashlib.sha256(b).hexdigest()==r['sha256']
(P/'STLINK-before-flash.log').write_bytes(subprocess.check_output([h.CLI,'-l','stlink']))
f=P/'hardware/previous-N8E-readback.bin'
body=h.IDENTITY+'''p r3_result
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
'''+f'dump binary memory {h.win(f).replace(chr(92),"/")} 0x08000000 {hex(0x08000000+len(old_image.read_bytes()))}\n'
h.debug('before-N8F-flash-image-and-stopped-state',old_elf,body);assert f.read_bytes()==old_image.read_bytes()
h.flash('flash-prepared-N8F8MHz',P/'arm/r3_coex.bin',a['code_sha'])
f=P/'hardware/N8F-readback.bin';body=h.IDENTITY+'''p r3_result
p r3_clock_fault
p/x *(unsigned int*)0x46002008
if r3_result.stage!=2 || r3_result.error || r3_result.run || fifo.running || r3_writer.active || r3_command || r3_clock_fault || ((*(unsigned int*)0x46002008>>28)&7)!=0
 quit 8
end
'''+f'dump binary memory {h.win(f).replace(chr(92),"/")} 0x08000000 {hex(0x08000000+a["images"][0]["bytes"])}\n'
h.debug('N8F-readback-and-WAIT8MHz',P/'arm/r3_coex.elf',body);assert f.read_bytes()==(P/'arm/r3_coex.bin').read_bytes()
(P/'FLASH_VERIFIED.json').write_text(json.dumps(dict(UID='001B0046/38425018/20333157',SN=h.SN,previous_N8E_entire_BIN_preserved=True,new_N8F_entire_BIN_identical=True,WAIT2=True,normal_programming_reset_only=True,no_extra_reset=True,no_mass_erase=True,no_OptionBytes=True,no_long=True),indent=2)+'\n');print('N8F8MHz normal flash + readback exact; WAIT2. N8E0003 preserved.')
