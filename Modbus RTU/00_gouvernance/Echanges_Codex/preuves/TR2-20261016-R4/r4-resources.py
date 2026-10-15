from pathlib import Path
import subprocess as s,json
root=Path.cwd();fw=root/'Modbus RTU/05_Firmware';out=Path('/home/lolo/dev/msm/reports/TR2-r4/resources');out.mkdir(exist_ok=True)
base=['arm-none-eabi-gcc','-std=c11','-mcpu=cortex-m33','-mthumb','-O0','-fstack-usage','-Wall','-Wextra','-Werror','-I',str(fw/'include'),'-I',str(fw/'platform/stm32'),'-I',str(root/'experiments/iis3dwb_st_reference/src'),'-I',str(root/'experiments/iis3dwb_st_reference/vendor/st')]
probe=out/'probe.c';probe.write_text('#include "live_fifo_acquisition.h"\n#include "tr2/common/cooperative_work.h"\nchar work_bytes[sizeof(Tr2CooperativeWork)];\nchar acquisition_bytes[sizeof(LiveFifoAcquisition)];\nchar fifo_bytes[sizeof(FifoLive)];\nchar engine_bytes[sizeof(LiveCommandEngine)];\nchar store_bytes[sizeof(LiveCommandStore)];\nchar configuration_bytes[sizeof(LiveConfiguration)];\nchar media_bytes[sizeof(TransactionalImageMedia)];\n')
cmds=[]
for name,source in [('probe',probe),('cooperative_work',fw/'src/common/cooperative_work.c'),('transactional_image_media',fw/'src/persistence/transactional_image_media.c'),('sha256',fw/'src/common/sha256.c'),('live_configuration',fw/'src/application/live_configuration.c'),('live_command_store',fw/'src/persistence/live_command_store.c'),('live_command_engine',fw/'src/application/live_command_engine.c'),('fifo_live',root/'experiments/iis3dwb_st_reference/src/fifo_live.c'),('live_fifo_acquisition',fw/'platform/stm32/live_fifo_acquisition.c')]:
 cmd=base+['-c',str(source),'-o',str(out/(name+'.o'))];s.run(cmd,check=True,capture_output=True);cmds.append(cmd)
(out/'commands.json').write_text(json.dumps(cmds,indent=2)+'\n');(out/'sizes.txt').write_bytes(s.check_output(['arm-none-eabi-nm','-S','--size-sort',str(out/'probe.o')]))
for name,elf in [('canonical',fw/'build-stm32-cumulative/tr2-cumulative-m014-m015-spi4.elf'),('bench',out.parent/'bench-build/tr2-r4-qualification-NON-PRODUCTION.elf')]:
 (out/(name+'-symbols.txt')).write_bytes(s.check_output(['arm-none-eabi-nm','-l',str(elf)]));(out/(name+'-size.txt')).write_bytes(s.check_output(['arm-none-eabi-size',str(elf)]))
print((out/'sizes.txt').read_text())
