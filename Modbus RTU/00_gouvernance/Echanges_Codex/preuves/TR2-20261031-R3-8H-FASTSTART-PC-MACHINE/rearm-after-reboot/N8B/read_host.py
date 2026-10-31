from pathlib import Path
import subprocess,json
P=Path(__file__).parent;PS='/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe'
s="""$ErrorActionPreference='Stop';[Console]::OutputEncoding=[Text.UTF8Encoding]::new($false);$ports=@(Get-CimInstance Win32_SerialPort|Select DeviceID,PNPDeviceID);$parents=@();foreach($p in $ports){if($p.DeviceID -eq 'COM5'){$parents+=Get-PnpDeviceProperty -InstanceId $p.PNPDeviceID -KeyName DEVPKEY_Device_Parent|Select InstanceId,Data}};[pscustomobject]@{Power=@(Get-CimInstance -Namespace root/wmi -ClassName BatteryStatus|Select PowerOnline,Discharging,Charging);Temp=$env:TEMP;Ports=$ports;Parents=$parents}|ConvertTo-Json -Depth 6"""
(P/'read-host.ps1').write_text(s)
z=subprocess.run([PS,'-NoProfile','-NonInteractive','-Command',s],capture_output=True,timeout=25);(P/'native-host.json').write_bytes(z.stdout);(P/'read-host.stderr').write_bytes(z.stderr);z.check_returncode();v=json.loads(z.stdout.decode('utf-8-sig'));assert any(p['PowerOnline'] for p in v['Power']);assert len(v['Parents'])==1 and '0032001F3235510C37333439' in v['Parents'][0]['Data']
for item,command in [('sleep','STANDBYIDLE'),('hibernate','HIBERNATEIDLE')]:
 z=subprocess.run([PS,'-NoProfile','-NonInteractive','-Command',f'powercfg /query SCHEME_CURRENT SUB_SLEEP {command}'],capture_output=True,timeout=20);z.check_returncode();(P/(item+'-policy.txt')).write_bytes(z.stdout)
print(json.dumps(v,indent=2),flush=True)
