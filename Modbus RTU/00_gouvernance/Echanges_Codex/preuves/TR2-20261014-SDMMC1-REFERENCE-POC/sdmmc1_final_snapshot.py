from pathlib import Path
import sys,subprocess,json
P=Path('/home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014');sys.path.insert(0,str(P));from hardware import *
elf=Path('/home/lolo/dev/msm/reports/SDMMC1-PoC-release-build/sdmmc1_poc.elf')
debug('final-original-release-state',elf,IDENTITY+'''p sd_ready
p/x hsd1.CID
p hsd1.SdCard
p/x hsd1.ErrorCode
p hsd1.State
p/x hsd1.Instance->CLKCR
p/x hsd1.Instance->RESP1
p _usbd_dev.connected
p _usbd_dev.cfg_num
p ejected
x/2wx 0xE000ED28
''');print((P/'final-original-release-state.log').read_bytes().decode(errors='replace'))
# Host observation after test, no further SD reads/writes requested by operator.
c=r'''[Console]::OutputEncoding=[Text.UTF8Encoding]::new($false); $n=@(Get-PnpDevice -PresentOnly | Where-Object {$_.InstanceId.Contains('VID_0483&PID_572B')} | Select-Object Status,FriendlyName,InstanceId); $d=@(Get-Disk | Where-Object {$_.FriendlyName -eq 'STM32 SD Card'} | Select-Object Number,FriendlyName,SerialNumber,Size,OperationalStatus,BusType,PartitionStyle); [ordered]@{utc=[DateTime]::UtcNow.ToString('o');devices=$n;disks=$d} | ConvertTo-Json -Depth 4'''
z=subprocess.run(['/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe','-NoProfile','-NonInteractive','-Command',c],capture_output=True,timeout=45);(P/'final-native-enumeration.json').write_bytes(z.stdout);z.check_returncode();print(z.stdout.decode())
