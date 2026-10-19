from pathlib import Path
import subprocess,json
p=Path('/home/lolo/dev/msm/reports/TR2-windows-trust-live-resume');r=Path('/home/lolo/dev/msm/projects/MSM_vibe-Windows-trust-live-resume/Modbus RTU/05_Firmware')
s=(Path('/home/lolo/dev/msm/reports/TR2-r5b2/model-final.c').read_text().replace('==70','==78').replace('run_floor=10','run_floor=11').replace('transaction_id=12','transaction_id=14').replace('transaction_id=13','transaction_id=15').replace('b->run==11','b->run==12').replace('!b->running&&b->store.generation==78','!b->running&&b->store.generation==86').replace('generation==91','generation==99').replace('journal78/root91/epoch5/run11','journal86/root99/epoch5/run12'))
(p/'model-cycle1.c').write_text(s)
core=(r/'CMakeLists.txt').read_text().split('add_library(tr2_core STATIC',1)[1].split(')',1)[0].split()
cmd=['cc','-std=c11','-O2','-Wall','-Wextra','-Werror','-I'+str(r/'include'),'-I'+str(r/'platform/stm32'),'-I'+str(r/'tests/platform'),str(p/'model-cycle1.c'),str(r/'platform/stm32/r4m_maintenance.c')]+[str(r/x) for x in core]+['-o',str(p/'model-cycle1')]
(p/'model-build-command.json').write_text(json.dumps(cmd,indent=2))
with (p/'model-build.log').open('wb') as f:subprocess.run(cmd,stdout=f,stderr=subprocess.STDOUT,check=True)
with (p/'model-cycle1.log').open('wb') as f:subprocess.run([str(p/'model-cycle1'),str(p/'hardware/fram-before.bin'),str(p/'model-cycle1.bin')],stdout=f,stderr=subprocess.STDOUT,check=True)
assert (p/'model-cycle1.bin').read_bytes()==(p/'hardware/cycle1-audit-fram.bin').read_bytes()
print('FULL_FRAM_CYCLE1_EXACT_TO_GENUINE_CORE_METADATA_MODEL')
