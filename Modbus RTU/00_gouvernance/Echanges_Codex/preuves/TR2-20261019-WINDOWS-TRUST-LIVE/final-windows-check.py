from pathlib import Path
import subprocess,json,hashlib,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-windows-trust-live-resume');app=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-LIVE-DEMO-20261019/app');m=json.loads((p/'checkpoint-gate/application-sha256.json').read_text());assert len(m)==377
assert {f.relative_to(app).as_posix():hashlib.sha256(f.read_bytes()).hexdigest() for f in app.rglob('*') if f.is_file()}==m
ps=r'''
[Console]::OutputEncoding=New-Object System.Text.UTF8Encoding($false)
$ErrorActionPreference='Stop'
$events=@(Get-WinEvent -FilterHashtable @{LogName='Microsoft-Windows-CodeIntegrity/Operational';Id=3033,3077;StartTime=[DateTimeOffset]::Parse('2026-10-19T13:35:00Z').UtcDateTime} -ErrorAction SilentlyContinue | Where-Object {$_.Message -like '*TR2-LIVE-DEMO-20261019\app\*'} | ForEach-Object {[pscustomobject]@{Id=$_.Id;Utc=$_.TimeCreated.ToUniversalTime().ToString('o');Message=$_.Message;Xml=$_.ToXml()}})
$processes=@(Get-CimInstance Win32_Process | Where-Object {$_.Name -match 'TR2.Supervision|ST-LINK_gdbserver|arm-none-eabi-gdb' } | Select-Object Name,ProcessId,CommandLine)
$s=[IO.Ports.SerialPort]::new('COM4',115200,[IO.Ports.Parity]::Even,8,[IO.Ports.StopBits]::One)
try{$s.Open();$free=$s.IsOpen}finally{$s.Dispose()}
[pscustomobject]@{Utc=[datetime]::UtcNow.ToString('o');SmartAppControlState=(Get-ItemProperty 'HKLM:\SYSTEM\CurrentControlSet\Control\CI\Policy').VerifiedAndReputablePolicyState;BlockedCount=$events.Count;Events=$events;Processes=$processes;COM4ExclusiveOpenWithoutTransmission=$free} | ConvertTo-Json -Depth 7
'''
cmd=['/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe','-NoProfile','-Command',ps]
with (p/'final-windows-check.json').open('wb') as f:subprocess.run(cmd,stdout=f,stderr=subprocess.STDOUT,timeout=30,check=True)
x=json.loads((p/'final-windows-check.json').read_bytes().decode('utf-8-sig'));assert x['SmartAppControlState']==1 and x['BlockedCount']==0 and not x['Processes'] and x['COM4ExclusiveOpenWithoutTransmission']
x.update(application_files_unchanged=377,no_security_change=True,no_serial_transmission_in_final_open_check=True)
(p/'final-deployment-summary.json').write_text(json.dumps(x,indent=2)+'\n');print('PASS SAC1, no correlated blocks,377 app hashes exact, COM4 free, no service/debug processes')
