from pathlib import Path
import subprocess as s,shlex,json,hashlib
root=Path.cwd();fw=root/'Modbus RTU/05_Firmware';build=fw/'build-stm32-cumulative';out=Path('/home/lolo/dev/msm/reports/TR2-r3b/resources');out.mkdir(exist_ok=True)
base=['arm-none-eabi-gcc','-std=c11','-mcpu=cortex-m33','-mthumb','-O0','-fstack-usage','-Wall','-Wextra','-Werror','-I',str(fw/'include'),'-I',str(fw/'platform/stm32'),'-I',str(root/'experiments/iis3dwb_st_reference/src'),'-I',str(root/'experiments/iis3dwb_st_reference/vendor/st')]
probe=out/'probe.c';probe.write_text('#include "live_fifo_acquisition.h"\nchar metrology_bytes[sizeof(LiveMetrology)];\nchar acquisition_bytes[sizeof(LiveFifoAcquisition)];\nchar engine_bytes[sizeof(LiveCommandEngine)];\nchar statistics_bytes[sizeof(AcquisitionWindow)];\n')
cmds=[]
for name,source in [('probe',probe),('live_metrology',fw/'src/application/live_metrology.c'),('window_statistics',fw/'src/domain/acquisition/window_statistics.c'),('live_fifo_acquisition',fw/'platform/stm32/live_fifo_acquisition.c'),('live_command_engine',fw/'src/application/live_command_engine.c')]:
 cmd=base+['-c',str(source),'-o',str(out/(name+'.o'))];s.run(cmd,check=True,capture_output=True);cmds.append(cmd)
(out/'commands.json').write_text(json.dumps(cmds,indent=2)+'\n');(out/'sizes.txt').write_bytes(s.check_output(['arm-none-eabi-nm','-S','--size-sort',str(out/'probe.o')]))
# Resolve new service + actual STM32 binding in a separate forced-link artifact.
commands=s.check_output(['ninja','-C',str(build),'-t','commands','tr2_stm32_p11c']).decode().splitlines()
line=next(x for x in reversed(commands) if ' -o tr2-cumulative-' in x and 'gcc' in x)
tokens=shlex.split(line);begin=next(i for i,x in enumerate(tokens) if x.endswith('arm-none-eabi-gcc'));end=next((i for i in range(begin,len(tokens)) if tokens[i]=='&&'),len(tokens));link=tokens[begin:end]
link[link.index('-o')+1]=str(out/'R3B-force-linked-NOT-FOR-FLASH.elf')
for i,x in enumerate(link):
 if x.startswith('-Wl,-Map='):link[i]='-Wl,-Map='+str(out/'R3B-force-linked-NOT-FOR-FLASH.map')
link+=['-Wl,-u,stm32_live_acquisition_attach,-u,stm32_live_acquisition_poll,-u,live_fifo_acquisition_bind_metrology,-u,live_fifo_acquisition_bind_engine,-u,live_metrology_project_b3,-u,live_metrology_ack']
p=s.run(link,cwd=build,capture_output=True,text=True);(out/'force-link.log').write_text(shlex.join(link)+'\n'+p.stdout+p.stderr);assert p.returncode==0
elf=out/'R3B-force-linked-NOT-FOR-FLASH.elf';symbols=s.check_output(['arm-none-eabi-nm','-l',str(elf)]).decode();(out/'forced-symbols.txt').write_text(symbols)
for sym in ['live_metrology_sample','live_metrology_project_b3','live_fifo_acquisition_bind_metrology','stm32_live_acquisition_attach']:
 assert any(' '+sym+'\t' in line or line.endswith(' '+sym) for line in symbols.splitlines()),sym
(out/'force-size.txt').write_bytes(s.check_output(['arm-none-eabi-size',str(elf)]));(out/'force-sha256.txt').write_text(hashlib.sha256(elf.read_bytes()).hexdigest()+'\n')
prod=build/'tr2-cumulative-m014-m015-spi4.elf';(out/'canonical-symbols.txt').write_bytes(s.check_output(['arm-none-eabi-nm','-l',str(prod)]));(out/'canonical-size.txt').write_bytes(s.check_output(['arm-none-eabi-size',str(prod)]))
print((out/'sizes.txt').read_text());print((out/'force-size.txt').read_text());print((out/'force-sha256.txt').read_text())
