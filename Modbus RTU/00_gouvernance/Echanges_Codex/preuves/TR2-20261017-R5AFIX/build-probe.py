from pathlib import Path
import subprocess
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R5AFIX-b3-wire');p=Path('/home/lolo/dev/msm/reports/TR2-r5afix');src=r/'Modbus RTU/05_Firmware/platform/stm32';b=p/'probe'
c=['cmake','-G','Ninja','-S',str(src),'-B',str(b),'-DCMAKE_TOOLCHAIN_FILE='+str(src/'cmake/arm-none-eabi-gcc.cmake'),'-DSTM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0','-DTR2_STM32_BUILD_PROFILE=CUMULATIVE','-DTR2_COMMISSIONING_MANIFEST='+str(r/'Modbus RTU/00_gouvernance/COMMISSIONING_M014_H.json'),'-DTR2_COMMISSIONING_REGISTRY='+str(r/'Modbus RTU/00_gouvernance/REGISTRE_ATTRIBUTION_TR2.json')]+['-DTR2_STM32_'+a+'=ON' for a in ['R4_BENCH','R4M_MAINTENANCE','R4P_COEXISTENCE','R4R_BENCH','R4S_BENCH','R4U_RUNTIME','R5AFIX_B3']]
with (p/'probe-build.log').open('w') as f:
 subprocess.run(c,stdout=f,stderr=subprocess.STDOUT,check=True);subprocess.run(['cmake','--build',str(b)],stdout=f,stderr=subprocess.STDOUT,check=True)
