from pathlib import Path
import subprocess,time,json
P=Path(__file__).parent
SN='0032001F3235510C37333439'
PRIMARY_SN='003500463235510B37333439'
CLI='/mnt/c/ST/STM32CubeCLT_1.22.0/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe'
SERVER='/mnt/c/ST/STM32CubeCLT_1.22.0/STLink-gdb-server/bin/ST-LINK_gdbserver.exe'
GDB='/mnt/c/ST/STM32CubeCLT_1.22.0/GNU-tools-for-STM32/bin/arm-none-eabi-gdb.exe'
def guard():
 c=json.loads((P/'wiring-confirmed.json').read_text())
 assert c['status']=='CONFIRMED' and c['spi_nets_exact'] and c['gnd_contact']
 assert 3.0<=c['vdd_volts']<=3.6 and 3.0<=c['vddio_volts']<=min(3.6,c['vdd_volts']+0.1)
 assert c['ribbon_orientation_confirmed'] and c['no_other_modules']
 assert SN!=PRIMARY_SN
 return c
def win(f):return subprocess.check_output(['wslpath','-w',str(f)],text=True).strip()
def debug(name,body,elf=None):
 guard()
 script=P/(name+'.gdb')
 if script.exists():raise RuntimeError('Evidence path exists; no automatic repeat')
 script.write_text('set pagination off\nset confirm off\nset remotetimeout 10\ntarget extended-remote localhost:61234\n'+body+'\ndetach\nquit\n')
 with (P/(name+'-server.log')).open('xb') as log:
  proc=subprocess.Popen([SERVER,'-d','-g','-i',SN,'--frequency','8000','-p','61234','-cp',r'C:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin'],stdout=log,stderr=subprocess.STDOUT)
  try:
   time.sleep(2)
   cmd=[GDB,'-nx','-q','-batch','-x',win(script)]
   if elf is not None:cmd.append(win(elf))
   out=subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=45)
   (P/(name+'.log')).write_bytes(out.stdout)
   code=proc.wait(timeout=15)
   (P/(name+'-invocation.json')).write_text(json.dumps({'command':cmd,'sn':SN,'gdb_exit':out.returncode,'server_exit':code},indent=2)+'\n')
   out.check_returncode()
   assert code==0
  finally:
   if proc.poll() is None:
    proc.terminate();proc.wait(timeout=10)
