from pathlib import Path
import subprocess,time,hashlib,json
P=Path(__file__).parent
SN='003500463235510B37333439'
CLI='/mnt/c/ST/STM32CubeCLT_1.22.0/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe'
SERVER='/mnt/c/ST/STM32CubeCLT_1.22.0/STLink-gdb-server/bin/ST-LINK_gdbserver.exe'
GDB='/mnt/c/ST/STM32CubeCLT_1.22.0/GNU-tools-for-STM32/bin/arm-none-eabi-gdb.exe'
def win(f):return subprocess.check_output(['wslpath','-w',str(f)],text=True).strip()
def debug(name,elf,body):
 script=P/(name+'.gdb');script.write_text('set pagination off\nset confirm off\nset remotetimeout 10\ntarget extended-remote localhost:61234\n'+body+'\ndetach\nquit\n')
 with (P/(name+'-server.log')).open('wb') as log:
  proc=subprocess.Popen([SERVER,'-d','-g','-i',SN,'--frequency','8000','-p','61234','-cp',r'C:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin'],stdout=log,stderr=subprocess.STDOUT);time.sleep(2)
  r=subprocess.run([GDB,'-nx','-q','-batch','-x',win(script),win(elf)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
  (P/(name+'.log')).write_bytes(r.stdout);print(r.stdout.decode('cp1252'));print('serverexit',proc.wait(timeout=15));r.check_returncode()
