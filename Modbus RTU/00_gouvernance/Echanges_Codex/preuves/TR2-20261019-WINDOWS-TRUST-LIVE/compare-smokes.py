from pathlib import Path
import subprocess,json,time,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-windows-trust-live-resume');wroot=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-WINDOWS-TRUST-20261019');assert not wroot.exists();wroot.mkdir()
win=lambda f:subprocess.check_output(['wslpath','-w',str(f)],text=True).strip();ps='/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe'
for name,exe in [('r5b2-reference',Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-r5b/TR2.Supervision.Service.exe')),('new-operator',Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-LIVE-DEMO-20261019/app/TR2.Supervision.Service.exe'))]:
 d=wroot/name;d.mkdir();cmd=[str(exe),'--live-direct','COM99','--allow-diagnostic','--database',win(d/'live.db'),'--listen','http://127.0.0.1:5102','--trace',win(d/'serial.jsonl'),'--seconds','5','--unit','1','--connection-control',win(d/'control')];start=datetime.datetime.now(datetime.timezone.utc).isoformat()
 (p/(name+'-command.json')).write_text(json.dumps(cmd,indent=2)+'\n')
 with (p/(name+'-console.log')).open('wb') as f:
  proc=subprocess.Popen(cmd,stdout=f,stderr=subprocess.STDOUT)
  try:
   time.sleep(1)
   code=r'''try {$r=Invoke-WebRequest -UseBasicParsing -Uri 'http://127.0.0.1:5102/api/v2/live/overview' -TimeoutSec 2;[pscustomobject]@{Status=[int]$r.StatusCode;Body=$r.Content}|ConvertTo-Json -Depth 5} catch {[pscustomobject]@{HttpUnavailable=$true;Error=$_.Exception.Message}|ConvertTo-Json}'''
   with (p/(name+'-http.json')).open('wb') as http:subprocess.run([ps,'-NoProfile','-Command',code],stdout=http,stderr=subprocess.STDOUT,timeout=8)
   proc.wait(timeout=20)
  finally:
   if proc.poll() is None:proc.terminate();proc.wait(timeout=5)
 end=datetime.datetime.now(datetime.timezone.utc).isoformat();(p/(name+'-result.json')).write_text(json.dumps(dict(start_utc=start,end_utc=end,return_code=proc.returncode,serial_port='COM99 nonexistent',hardware=False,exe=win(exe)),indent=2)+'\n');print(name,proc.returncode,flush=True)
script=r'''Get-WinEvent -FilterHashtable @{LogName='Microsoft-Windows-CodeIntegrity/Operational';Id=3033,3077,3089,3090,3091,3092;StartTime=(Get-Date).AddMinutes(-5)} -ErrorAction SilentlyContinue | Where-Object {$_.Message -match 'TR2-r5b|TR2-LIVE-DEMO|TR2.Domain.dll'} | ForEach-Object { [pscustomobject]@{Id=$_.Id;RecordId=$_.RecordId;TimeCreatedUtc=$_.TimeCreated.ToUniversalTime().ToString('o');Xml=$_.ToXml();Message=$_.Message} } | ConvertTo-Json -Depth 4'''
with (p/'smoke-correlated-events.json').open('wb') as f:subprocess.run([ps,'-NoProfile','-Command',script],stdout=f,stderr=subprocess.STDOUT,timeout=20)
