from pathlib import Path
import subprocess as s
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4V-rtu-latency');p=Path('/home/lolo/dev/msm/reports/TR2-r4v');src=r/'Modbus RTU/05_Firmware/platform/stm32';import sys
pace=sys.argv[1];b=p/('probe-'+pace)
c=['cmake','-G','Ninja','-S',str(src),'-B',str(b),'-DCMAKE_TOOLCHAIN_FILE='+str(src/'cmake/arm-none-eabi-gcc.cmake'),'-DSTM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0','-DTR2_STM32_BUILD_PROFILE=CUMULATIVE','-DTR2_COMMISSIONING_MANIFEST='+str(r/'Modbus RTU/00_gouvernance/COMMISSIONING_M014_H.json'),'-DTR2_COMMISSIONING_REGISTRY='+str(r/'Modbus RTU/00_gouvernance/REGISTRE_ATTRIBUTION_TR2.json')]+['-DTR2_STM32_'+a+'=ON' for a in ['R4_BENCH','R4M_MAINTENANCE','R4P_COEXISTENCE','R4R_BENCH','R4S_BENCH','R4U_RUNTIME','R4V_LATENCY']]
c+=['-DTR2_STM32_R4V_PACING_US='+pace]
with (p/('probe-'+pace+'.log')).open('w') as f:
 s.run(c,stdout=f,stderr=s.STDOUT,check=True);s.run(['cmake','--build',str(b)],stdout=f,stderr=s.STDOUT,check=True)
