from pathlib import Path
import sys,json,hashlib,subprocess
P=Path(__file__).parent;sys.path.insert(0,str(P));import hardware_second as h
R=Path('/home/lolo/dev/msm/projects/MSM_vibe-R3-8h-faststart');old=Path('/home/lolo/dev/msm/reports/TR2-r3-8h-faststart-N8D-20261031/arm');previous=(old/'r3_coex.bin').read_bytes();image=P/'arm/r3_coex.bin';b=image.read_bytes()
assert hashlib.sha256(b).hexdigest()=='c31904feca48fd06d6102eee40b54b373bbfe5e9896d0ddc007434cf253ddc17'
published=subprocess.check_output(['git','show','120842b0:Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/arm/r3_coex.bin'],cwd=R);assert published==b
before=P/'hardware/previous-D-before-restore.bin'
h.debug('before-restore-identity-idle-reset-flags',old/'r3_coex.elf',h.IDENTITY+f'''p r3_result
p r3_clock_fault
p/x *(unsigned int*)0x46020cf4
x/3wx 0x46020c1c
x/3wx 0x46020ce0
x/6wx 0x42020800
x/2wx 0x42020820
if fifo.running || r3_writer.active || r3_command || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end
dump binary memory {h.win(before).replace(chr(92),'/')} 0x08000000 0x{0x08000000+len(previous):x}
''');assert before.read_bytes()==previous
h.flash('restore-exact-N8A',image,'d5af0828e370780320bda67fac5d537bf3fd5016')
after=P/'hardware/restored-N8A-readback.bin'
h.debug('restored-N8A-entire-readback-WAIT',P/'arm/r3_coex.elf',h.IDENTITY+f'''p r3_result
p r3_clock_fault
p/x *(unsigned int*)0x46020cf4
if r3_result.stage!=2 || r3_result.run || r3_result.error || r3_clock_fault || r3_command || fifo.running || r3_writer.active || !(*(unsigned int*)0x42021414 &0x1000)
 quit 9
end
dump binary memory {h.win(after).replace(chr(92),'/')} 0x08000000 0x{0x08000000+len(b):x}
''');assert after.read_bytes()==b
(P/'RESTORED.json').write_text(json.dumps(dict(exact_original_N8A_BIN=True,sha256=hashlib.sha256(b).hexdigest(),bytes=len(b),source_sha='d5af0828e370780320bda67fac5d537bf3fd5016',UID='001B0046/38425018/20333157',SN=h.SN,one_standard_flash_with_programming_reset=True,no_extra_reset=True,stage=2,run=0,old_image_preserved=True,GDB_detached=True,no_SD_command_or_START_yet=True),indent=2)+'\n')
print('Exact N8A restored, full readback identical, WAIT safe; no new build',flush=True)
