from hardware import *
R=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4U-live-runtime');B=P.parent/'probe-build-03';elf=B/'tr2-r4u-live-runtime-NON-PRODUCTION.elf';image=elf.with_suffix('.bin');code=subprocess.check_output(['git','rev-parse','HEAD'],cwd=R,text=True).strip()
assert code.startswith('0f1fa5d') and subprocess.check_output(['git','rev-parse','origin/main'],cwd=R,text=True).strip()==code
for n in ['Modbus RTU/05_Firmware/platform/stm32/stm32_r4u_runtime.c','Modbus RTU/05_Firmware/LIVE_R4U.md']:
 assert subprocess.check_output(['git','show','origin/main:'+n],cwd=R)==(R/n).read_bytes()
assert hashlib.sha256(image.read_bytes()).hexdigest()=='9722d755ed0c9b9f303cf17b0761c169bec6062882893b4f0b04a4d92b4cb56b'
assert (P/'ro02-fram.bin').read_bytes()==(P/'fram-before.bin').read_bytes()
flash('image03-flash',image,code)
debug('image03-boot-check',elf,IDENTITY+f'''dump binary memory {win(P/'image03-readback.bin')} 0x08000000 0x08000000+{image.stat().st_size}
monitor reset
shell sleep 1
p r4u_result
if r4u_result.stage != 1 || r4u_result.error != 0
 quit 4
end
''',40)
assert (P/'image03-readback.bin').read_bytes()==image.read_bytes()
