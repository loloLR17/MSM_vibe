from pathlib import Path
import subprocess,json,hashlib,datetime,time,re
from hardware import P,debug,IDENTITY,win
ELF=Path('/home/lolo/dev/msm/reports/SDMMC1-PoC-release-build/sdmmc1_poc.elf')
IMAGE=ELF.with_suffix('.bin')
POWERSHELL='/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe'
STATE='''p sd_ready
p/x hsd1.CID
p hsd1.SdCard.LogBlockNbr
p hsd1.SdCard.LogBlockSize
p/x hsd1.ErrorCode
p hsd1.State
p/x hsd1.Instance->CLKCR
p/x hsd1.Instance->RESP1
p/x hsd1.Instance->STA
p SystemCoreClock
p/x ((RCC_TypeDef *)0x46020c00)->CR
p/x ((RCC_TypeDef *)0x46020c00)->CCIPR1
p/x ((RCC_TypeDef *)0x46020c00)->CCIPR2
p _usbd_dev.connected
p _usbd_dev.cfg_num
p ejected
x/2wx 0xE000ED28
'''
GUARD='''if sd_ready != 1 || hsd1.CID[0] != 0x03534453 || hsd1.CID[1] != 0x44333247 || hsd1.CID[2] != 0x858bf092 || hsd1.CID[3] != 0x3C01927A || hsd1.SdCard.LogBlockNbr != 62333952 || hsd1.SdCard.LogBlockSize != 512 || hsd1.Instance->CLKCR != 0x4002 || ( ((RCC_TypeDef *)0x46020c00)->CCIPR1 & 0x0c000000) != 0 || ( ((RCC_TypeDef *)0x46020c00)->CCIPR2 & 0x4000) != 0 || (((RCC_TypeDef *)0x46020c00)->CR & 0x3000) != 0x3000
 quit 4
end
'''
def ps(name,command,timeout=45):
 assert not (P/(name+'.json')).exists()
 z=subprocess.run([POWERSHELL,'-NoProfile','-NonInteractive','-Command',command],capture_output=True,timeout=timeout)
 (P/(name+'.json')).write_bytes(z.stdout);(P/(name+'-stderr.log')).write_bytes(z.stderr)
 (P/(name+'-invocation.json')).write_text(json.dumps({'command':command,'return_code':z.returncode,'utc':datetime.datetime.now(datetime.timezone.utc).isoformat()},indent=2)+'\n')
 print(name,z.returncode,z.stdout.decode(errors='replace'),flush=True)
 z.check_returncode();return json.loads(z.stdout)
def host(name):
 return ps(name,r'''[Console]::OutputEncoding=[Text.UTF8Encoding]::new($false);$ErrorActionPreference='Stop'; $n=@(Get-PnpDevice -PresentOnly | Where-Object {$_.InstanceId.Contains('VID_0483&PID_572B')} | Select-Object Status,FriendlyName,InstanceId); $d=@(Get-Disk | Where-Object {$_.FriendlyName -eq 'STM32 SD Card' -and $_.SerialNumber.Trim() -eq '123456'} | Select-Object Number,FriendlyName,SerialNumber,Size,OperationalStatus,BusType,PartitionStyle); [ordered]@{utc=[DateTime]::UtcNow.ToString('o');devices=$n;disks=$d} | ConvertTo-Json -Depth 4''')
def snapshot(name,extra='',guard=False):
 debug(name,ELF,IDENTITY+STATE+(GUARD if guard else '')+extra)
 print((P/(name+'.log')).read_bytes().decode(errors='replace'),flush=True)
 s=(P/(name+'.log')).read_text(errors='replace')
 # Named values also saved to make the raw numbered GDB readings unambiguous.
 return s
if __name__=='__main__':
 import sys
 op=sys.argv[1]
 if op=='initial':
  host('host-before-debug')
  snapshot('initial-identity-image-state','dump binary memory '+win(P/'initial-flash-readback.bin')+' 0x08000000 '+hex(0x08000000+IMAGE.stat().st_size)+'\n',True)
  q=(P/'initial-flash-readback.bin').read_bytes();assert q==IMAGE.read_bytes()
  (P/'image-verification.json').write_text(json.dumps({'bytes':len(q),'readback_sha256':hashlib.sha256(q).hexdigest(),'expected_sha256':'2a2fe744ab926789ae180b74f2a250b2a9202329b5afde51a1155cdd69d54c1a','byte_equal':True,'flash_executed':False,'reset_executed':False,'elf_sha256':hashlib.sha256(ELF.read_bytes()).hexdigest()},indent=2)+'\n')
  host('host-after-initial-debug')
 elif op=='recover':
  snapshot('initial-no-media-recovery','monitor reset\n')
  time.sleep(8)
  snapshot('after-initial-recovery-preflight',guard=True)
  host('host-after-initial-recovery')
 elif op=='snapshot':
  snapshot(sys.argv[2]);host(sys.argv[2]+'-host')
 else:raise ValueError(op)
