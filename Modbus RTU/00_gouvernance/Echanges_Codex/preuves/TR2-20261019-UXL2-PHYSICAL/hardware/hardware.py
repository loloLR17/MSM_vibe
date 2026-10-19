from pathlib import Path
import subprocess,time,hashlib,json,datetime
P=Path('/home/lolo/dev/msm/reports/TR2-uxl2-physical/hardware')
SN='003500463235510B37333439'
CLI='/mnt/c/ST/STM32CubeCLT_1.22.0/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe'
SERVER='/mnt/c/ST/STM32CubeCLT_1.22.0/STLink-gdb-server/bin/ST-LINK_gdbserver.exe'
GDB='/mnt/c/ST/STM32CubeCLT_1.22.0/GNU-tools-for-STM32/bin/arm-none-eabi-gdb.exe'
IDENTITY='''x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
'''
def win(path):return subprocess.check_output(['wslpath','-w',str(path)],text=True).strip()
def debug(name,elf,body,timeout=45,post_commands=None):
 script=P/(name+'.gdb');assert not script.exists();script.write_text('set pagination off\nset confirm off\nset remotetimeout 10\ntarget extended-remote localhost:61234\n'+body+'\ndetach\nquit\n')
 cmd=[SERVER,'-d','-g','-i',SN,'--frequency','8000','-p','61234','-cp',r'C:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin']
 with (P/(name+'-server.log')).open('wb') as log:
  proc=subprocess.Popen(cmd,stdout=log,stderr=subprocess.STDOUT)
  try:
   time.sleep(2)
   gdbcmd=[GDB,'-nx','-q','-batch','-x',win(script),win(elf)]
   if post_commands:
    for command in post_commands:gdbcmd.extend(['-ex',command])
   with (P/(name+'.log')).open('wb') as evidence:
    z=subprocess.run(gdbcmd,stdout=evidence,stderr=subprocess.STDOUT,timeout=timeout)
   result=dict(server=cmd,gdb=gdbcmd,return_code=z.returncode,utc=datetime.datetime.now(datetime.timezone.utc).isoformat());(P/(name+'-invocation.json')).write_text(json.dumps(result,indent=2)+'\n')
   proc.wait(timeout=15);z.check_returncode();print(name,'OK',flush=True)
  finally:
   if proc.poll() is None:proc.terminate();proc.wait(timeout=10)
def flash(name,image,source_sha):
 cmd=[CLI,'-c','port=SWD','sn='+SN,'freq=8000','-w',win(image),'0x08000000','-v','-rst']
 (P/(name+'-command.json')).write_text(json.dumps(dict(command=cmd,code_sha=source_sha,sha256=hashlib.sha256(image.read_bytes()).hexdigest(),normal_programming_only=True),indent=2)+'\n')
 z=subprocess.run(cmd,capture_output=True,timeout=55);(P/(name+'.log')).write_bytes(z.stdout+z.stderr);z.check_returncode();assert b'Download verified successfully' in z.stdout;print(name,'verified',flush=True)
