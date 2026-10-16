from pathlib import Path
import subprocess as s
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4S-service-budget-safe-recovery');b=Path('/home/lolo/dev/msm/reports/TR2-r4s/probe-build-01');src=root/'Modbus RTU/05_Firmware/platform/stm32'
cmd=['cmake','-G','Ninja','-S',str(src),'-B',str(b),'-DCMAKE_TOOLCHAIN_FILE='+str(src/'cmake/arm-none-eabi-gcc.cmake'),'-DSTM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0','-DTR2_STM32_BUILD_PROFILE=CUMULATIVE','-DTR2_COMMISSIONING_MANIFEST='+str(root/'Modbus RTU/00_gouvernance/COMMISSIONING_M014_H.json'),'-DTR2_COMMISSIONING_REGISTRY='+str(root/'Modbus RTU/00_gouvernance/REGISTRE_ATTRIBUTION_TR2.json')]+['-DTR2_STM32_'+v+'=ON' for v in ['R4_BENCH','R4M_MAINTENANCE','R4P_COEXISTENCE','R4R_BENCH','R4S_BENCH']]
with (b.parent/'probe-build-01.log').open('w') as f:
 s.run(cmd,stdout=f,stderr=s.STDOUT,check=True);s.run(['cmake','--build',str(b)],stdout=f,stderr=s.STDOUT,check=True)
