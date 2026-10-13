from pathlib import Path
import subprocess,os,json
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-A04-01');p=Path('/tmp/tr2-a0401-proof').read_text();p=Path(p);env=dict(os.environ,STM32CUBE_U5_ROOT='/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0')
commands=[('profile-tests-final',['python3','-B','Modbus RTU/05_Firmware/tests/platform/test_stm32_build_profiles.py','--evidence-dir',str(p/'configurations-final'),'-v']),('cumulative-validator-final',['bash','./tr2_validate.sh','--cross-build-only'])]
results=[]
for label,cmd in commands:
 with (p/(label+'.log')).open('wb') as log:z=subprocess.run(cmd,cwd=r,env=env,stdout=log,stderr=subprocess.STDOUT)
 results.append(dict(label=label,command=cmd,working_directory=str(r),environment={'STM32CUBE_U5_ROOT':env['STM32CUBE_U5_ROOT']},return_code=z.returncode))
 print(label,z.returncode,flush=True)
 assert z.returncode==0,label
(p/'validation-invocations.json').write_text(json.dumps(results,indent=2)+'\n')
