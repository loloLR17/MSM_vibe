from pathlib import Path
import subprocess,json,os,hashlib
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-A04-01');p=Path(Path('/tmp/tr2-a0401-proof').read_text());s=r/'Modbus RTU/05_Firmware/platform/stm32';sdk='/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0';results=[]
for label,flag,artifact in [('qualification-U','ST_FIFO_WINDOW','tr2-special-st_fifo_window-NON-CUMULATIVE'),('legacy','LEGACY_SD_HARNESS','tr2-legacy-SD-WRITES-NON-PRODUCTION')]:
 b=p/('build-'+label)
 cmds=[['cmake','-G','Ninja','-S',str(s),'-B',str(b),'-DCMAKE_TOOLCHAIN_FILE='+str(s/'cmake/arm-none-eabi-gcc.cmake'),'-DSTM32CUBE_U5_ROOT='+sdk,'-DTR2_STM32_'+flag+'=ON'],['cmake','--build',str(b),'-j4']]
 for i,cmd in enumerate(cmds):
  with (p/f'{label}-{i}.log').open('wb') as log:z=subprocess.run(cmd,stdout=log,stderr=subprocess.STDOUT)
  results.append(dict(command=cmd,return_code=z.returncode,log=f'{label}-{i}.log'));assert z.returncode==0,label
 print(label,'built',flush=True)
(p/'additional-build-commands.json').write_text(json.dumps(results,indent=2)+'\n')
