from pathlib import Path
import subprocess,json,datetime,time,hashlib
p=Path('/home/lolo/dev/msm/reports/TR2-windows-trust-live-resume');w=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-WINDOWS-TRUST-20261019');app=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-LIVE-DEMO-20261019/app');d=w/'verified-smoke';d.mkdir();win=lambda f:subprocess.check_output(['wslpath','-w',str(f)],text=True).strip();ps='/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe'
m=json.loads(Path('/home/lolo/dev/msm/reports/TR2-uxl2-physical/windows-deployment.json').read_text());n=0
for f in m['files']:
 if f['path'].startswith('app/'):
  path=app/f['path'][4:];assert hashlib.sha256(path.read_bytes()).hexdigest()==f['sha256'];n+=1
cmd=[str(app/'TR2.Supervision.Service.exe'),'--live-direct','COM99','--allow-diagnostic','--database',win(d/'live.db'),'--listen','http://127.0.0.1:5102','--trace',win(d/'serial.jsonl'),'--seconds','15','--unit','1','--connection-control',win(d/'control')];started=datetime.datetime.now(datetime.timezone.utc).isoformat();(p/'verified-smoke-command.json').write_text(json.dumps(cmd,indent=2)+'\n')
script=r'''$ErrorActionPreference='Stop';$deadline=(Get-Date).AddSeconds(12);$ok=$false;do {try {$a=Invoke-WebRequest -UseBasicParsing 'http://127.0.0.1:5102/api/v2/live/overview' -TimeoutSec 1;$ok=$a.StatusCode -eq 200} catch {Start-Sleep -Milliseconds 150}} while(-not $ok -and (Get-Date) -lt $deadline);if(-not $ok){throw 'New operator host HTTP unavailable'};$paths=@('/api/v1/fleet','/api/v2/live/overview','/api/v2/live/history?bus=live-direct&unit=1&from=2026-10-19T00:00:00Z&to=2026-10-20T00:00:00Z','/live-device.html?deviceId=1&view=vibration');$rows=@();foreach($path in $paths){$r=Invoke-WebRequest -UseBasicParsing ('http://127.0.0.1:5102'+$path) -TimeoutSec 3;$rows += [pscustomobject]@{Path=$path;Status=[int]$r.StatusCode;Body=$r.Content}};$rows | ConvertTo-Json -Depth 5'''
with (p/'verified-smoke-console.log').open('wb') as log:
 proc=subprocess.Popen(cmd,stdout=log,stderr=subprocess.STDOUT)
 try:
  with (p/'verified-smoke-http.json').open('wb') as f:probe=subprocess.run([ps,'-NoProfile','-Command',script],stdout=f,stderr=subprocess.STDOUT,timeout=25)
  proc.wait(timeout=30)
 finally:
  if proc.poll() is None:proc.terminate();proc.wait(timeout=5)
 assert probe.returncode==0 and proc.returncode==0,(probe.returncode,proc.returncode)
rows=json.loads((p/'verified-smoke-http.json').read_bytes().decode('utf-8-sig'));assert all(x['Status']==200 for x in rows)
query=r'''$start=[DateTimeOffset]::Parse('''+"'"+started+"'"+r''').UtcDateTime;$events=@(Get-WinEvent -FilterHashtable @{LogName='Microsoft-Windows-CodeIntegrity/Operational';Id=3033,3077;StartTime=$start} -ErrorAction SilentlyContinue | Where-Object {$_.Message -match 'TR2-LIVE-DEMO-20261019'} | ForEach-Object {[pscustomobject]@{Id=$_.Id;Utc=$_.TimeCreated.ToUniversalTime().ToString('o');Message=$_.Message}});[pscustomobject]@{BlockedCount=$events.Count;Events=$events;PolicyState=(Get-ItemProperty 'HKLM:\SYSTEM\CurrentControlSet\Control\CI\Policy').VerifiedAndReputablePolicyState} | ConvertTo-Json -Depth 5;fsutil.exe file queryEA 'C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-DEMO-20261019\app\TR2.Domain.dll' '''
with (p/'verified-smoke-events.log').open('wb') as f:subprocess.run([ps,'-NoProfile','-Command',query],stdout=f,stderr=subprocess.STDOUT,timeout=20)
text=(p/'verified-smoke-events.log').read_text(errors='replace');assert '"BlockedCount":  0' in text and '"PolicyState":  1' in text,text
summary=dict(result='PASS',start_utc=started,end_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),unchanged_application_files_verified=n,return_code=proc.returncode,http=[dict(path=x['Path'],status=x['Status']) for x in rows],correlated_block_events=0,smart_app_control_state=1,security_changes=False,serial='COM99 nonexistent',physical=False,old_and_new_both_currently_start=True,exact_ISG_decision_reason_not_audited=True,future_builds_not_automatically_authorized=True)
(p/'verified-smoke-summary.json').write_text(json.dumps(summary,indent=2)+'\n');print(summary)
