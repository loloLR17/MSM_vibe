from pathlib import Path
import subprocess,os,json
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-A04-02');p=Path(Path('/tmp/tr2-a0402-proof').read_text());env=dict(os.environ,STM32CUBE_U5_ROOT='/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0');cmd=['bash','./tr2_validate.sh']
with (p/'validation-full.log').open('wb') as log:z=subprocess.run(cmd,cwd=r,env=env,stdout=log,stderr=subprocess.STDOUT)
(p/'validation-full-command.json').write_text(json.dumps(dict(command=cmd,working_directory=str(r),environment={'STM32CUBE_U5_ROOT':env['STM32CUBE_U5_ROOT']},return_code=z.returncode),indent=2)+'\n')
print('Full validation return',z.returncode);assert z.returncode==0
