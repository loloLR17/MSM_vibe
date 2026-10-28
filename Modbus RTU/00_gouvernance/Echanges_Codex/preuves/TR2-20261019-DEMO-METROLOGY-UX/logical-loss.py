from pathlib import Path
import subprocess,json
p=Path(__file__).parent
ps=r'''[Console]::OutputEncoding=New-Object System.Text.UTF8Encoding($false)
$ErrorActionPreference='Stop'
$control='C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-COM4-HELD-20261019\data\connection-control'
$lock=[IO.File]::Open($control+'.command.lock',[IO.FileMode]::OpenOrCreate,[IO.FileAccess]::ReadWrite,[IO.FileShare]::None)
$token='disconnected:'+([Guid]::NewGuid()).ToString()
try {
 if(Test-Path $control){throw 'An existing control request must be preserved'}
 $view=Invoke-RestMethod 'http://127.0.0.1:5100/api/v2/live/overview'
 if(-not $view.devices[0].observation.snapshot.acquisitionStopped){throw 'STOP not observed; refusing pause'}
 [IO.File]::WriteAllText($control,$token)
 $end=(Get-Date).AddSeconds(10)
 while((Get-Date) -lt $end -and (-not(Test-Path ($control+'.released')) -or [IO.File]::ReadAllText($control+'.released') -ne $token)){Start-Sleep -Milliseconds 100}
 if(-not(Test-Path ($control+'.released')) -or [IO.File]::ReadAllText($control+'.released') -ne $token){throw 'No matching release acknowledgment'}
 [pscustomobject]@{Utc=[datetime]::UtcNow.ToString('o');Token=$token;Acknowledged=$true;PhysicalCableRemoved=$false;OperatorLockExclusive=$true}|ConvertTo-Json
 & 'C:\Program Files\nodejs\node.exe' '\\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-demo-metrology-ux\observe.cjs' '\\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-demo-metrology-ux\logical-loss-api.json'
 & 'C:\Program Files\nodejs\node.exe' '\\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-demo-metrology-ux\browser-states.cjs' '\\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-demo-metrology-ux' logical-loss lost
 if($LASTEXITCODE -ne 0){throw 'Browser failed'}
} finally {
 if((Test-Path $control) -and [IO.File]::ReadAllText($control) -eq $token){[IO.File]::Delete($control)}
 $lock.Dispose()
}
'''
(p/'logical-loss-command.json').write_text(json.dumps(dict(powershell=ps,reason='New UI must retain honest stale values on actual logical COM release; no cable disconnection, command, target reset or fake API'),indent=2))
with (p/'logical-loss.log').open('wb') as log:r=subprocess.run(['/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe','-NoProfile','-Command',ps],stdout=log,stderr=subprocess.STDOUT,timeout=50)
print('Logical pause/real released COM4/browser/resume:',r.returncode);assert r.returncode==0
