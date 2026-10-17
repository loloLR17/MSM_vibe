from hardware import *
R=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4U-live-runtime');B=P.parent/'probe-build-04';elf=B/'tr2-r4u-live-runtime-NON-PRODUCTION.elf';image=elf.with_suffix('.bin');code=subprocess.check_output(['git','rev-parse','HEAD'],cwd=R,text=True).strip()
assert code.startswith('806d967') and subprocess.check_output(['git','rev-parse','origin/main'],cwd=R,text=True).strip()==code
for n in ['Modbus RTU/05_Firmware/platform/stm32/stm32_r4u_runtime.c','Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md']:
 assert subprocess.check_output(['git','show','origin/main:'+n],cwd=R)==(R/n).read_bytes()
assert hashlib.sha256(image.read_bytes()).hexdigest()=='89951ff52ee997ff742ce0684fdbaff1cb22baef080aca872f9dd4e802056f63'
assert (P/'ro04-fram.bin').read_bytes()==(P/'fram-before.bin').read_bytes()
flash('image04-flash',image,code)
debug('image04-boot-check',elf,IDENTITY+f'''dump binary memory {win(P/'image04-readback.bin')} 0x08000000 0x08000000+{image.stat().st_size}
monitor reset
tbreak stm32_r4u_runtime.c:127
continue
p r4u_result
if r4u_result.stage != 1 || r4u_result.error != 0
 quit 4
end
''',40)
assert (P/'image04-readback.bin').read_bytes()==image.read_bytes()
body=IDENTITY+'''break r4u_checkpoint
set r4_trigger=0x52345501
continue
p r4u_result
p r4_result
p work
p runtime
p metric.sequence
p event_time
p r4r_result
p owner.acquisition.fifo.total_xyz
p owner.acquisition.fifo.error
p owner.acquisition.fifo.stop_error
'''+f'''set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
dump binary memory {win(P/'ram04-fram.bin')} baseline baseline+262144
'''+'''if $rr || r4u_result.stage != 3 || r4u_result.error || work.error || work.deadline_missed || r4_result.max_gap_us > 5000 || r4_result.write_bytes
 quit 5
end
'''
debug('ram04-phase',elf,body,60)
assert (P/'ram04-fram.bin').read_bytes()==(P/'fram-before.bin').read_bytes()
print('RAM04 FRAM exact',flush=True)
