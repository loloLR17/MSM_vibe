param(
 [ValidateSet('Prepare','Offline','Physical','Steady')][string]$Task,
 [string]$PackageName,[string]$ExpectedZipHash,[string]$ExpectedManifestHash,
 [string]$ExpectedB0Hash,[string]$GateUtc,
 [ValidateSet('physical-once','physical-after-b1-fix','physical-steady-final','physical-m015-a-restored','physical-m015-b-restored','physical-m015-c-restored','physical-m015-d-restored','physical-m015-f-restored','physical-m015-g-restored','physical-m015-h-restored','physical-m015-i-restored','physical-m015-j-restored')][string]$RunName='physical-once'
)
$ErrorActionPreference='Stop'; $ProgressPreference='SilentlyContinue'
[Console]::OutputEncoding=New-Object System.Text.UTF8Encoding($false)
$locked=$false; $mutex=$null; $process=$null
$physical=$Task -in @('Physical','Steady')
$maximum=if($Task -eq 'Steady'){2}else{9}
try {
 if($RunName -in @('physical-m015-a-restored','physical-m015-b-restored','physical-m015-c-restored','physical-m015-d-restored','physical-m015-f-restored','physical-m015-g-restored','physical-m015-h-restored','physical-m015-i-restored','physical-m015-j-restored') -and $Task -ne 'Steady'){throw 'M015 restoration permits only Steady'}
 if($env:COMPUTERNAME -ne 'LAPTOP-3F6J7STJ'){throw 'Unexpected HP identity'}
 if($PackageName -notmatch '^M014-H-[0-9TZ]+$'){throw 'Invalid unique package name'}
 foreach($hash in @($ExpectedZipHash,$ExpectedManifestHash,$ExpectedB0Hash)){
  if($hash -notmatch '^[a-fA-F0-9]{64}$'){throw 'Trusted SHA256 required'}
 }
 $root=Join-Path $env:LOCALAPPDATA 'TR2'; $package=Join-Path $root $PackageName
 $zip=$package+'.zip'
 if($Task -eq 'Prepare'){
  if(Test-Path -LiteralPath $package){throw 'Package directory already exists; no overwrite'}
  if((Get-FileHash -LiteralPath $zip -Algorithm SHA256).Hash -ne $ExpectedZipHash){throw 'ZIP mismatch'}
  Expand-Archive -LiteralPath $zip -DestinationPath $package
 }
 $manifest=Join-Path $package 'M014-H-package-manifest.json'
 if((Get-FileHash -LiteralPath $manifest -Algorithm SHA256).Hash -ne $ExpectedManifestHash){throw 'Manifest mismatch'}
 $files=@((Get-Content -LiteralPath $manifest -Raw|ConvertFrom-Json).files)
 if($files.Count -lt 300){throw 'Incomplete self-contained package'}
 foreach($file in $files){
  $path=[IO.Path]::GetFullPath((Join-Path $package $file.path))
  $prefix=[IO.Path]::GetFullPath($package).TrimEnd('\')+'\'
  if([IO.Path]::IsPathRooted($file.path) -or -not $path.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)){throw 'Unsafe package path'}
  $item=Get-Item -LiteralPath $path
  if($item.Length -ne $file.bytes -or (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ne $file.sha256){throw 'Package file mismatch'}
 }
 $b0=Join-Path $package 'expected-b0.json'
 if((Get-FileHash -LiteralPath $b0 -Algorithm SHA256).Hash -ne $ExpectedB0Hash){throw 'B0 mismatch'}
 if($Task -eq 'Prepare'){
  [ordered]@{task=$Task;package=$package;filesVerified=$files.Count;serialOpened=$false}|ConvertTo-Json;exit 0
 }
 $mutex=New-Object System.Threading.Mutex($false,'Global\TR2-COM7-Orchestration')
 try{$locked=$mutex.WaitOne(0)}catch [System.Threading.AbandonedMutexException] {throw 'Abandoned orchestration; inspect before physical retry'}
 if(-not $locked){throw 'Another COM7 orchestration active'}
 $candidates=@(Get-Process|Where-Object{$_.ProcessName -match 'TR2|dotnet|python|putty|terminal|modbus'})
 if($candidates.Count){throw 'Potential serial master already active'}
 $args='--commissioning-check --protocol-version 1 --expected-b0 "'+$b0+'" --port COM7 --baud 115200 --parity Even --timeout-ms 1000'
 $deadline=30000
 if($physical){
  $offline=Join-Path $package 'offline-success.json'
  if(-not(Test-Path -LiteralPath $offline)){throw 'No successful OFFLINE preflight'}
  $previous=Get-Content -LiteralPath $offline -Raw|ConvertFrom-Json
  if($previous.manifest -ne $ExpectedManifestHash){throw 'OFFLINE applies to different package'}
  $age=([DateTimeOffset]::UtcNow-[DateTimeOffset]::Parse($GateUtc)).TotalSeconds
  if($age -lt 0 -or $age -gt 30){throw 'Stale target baseline; no port opened'}
  $ports=@(Get-CimInstance Win32_PnPEntity|Where-Object{$_.Name -match '\(COM7\)' -and $_.Status -eq 'OK' -and $_.Service -eq 'FTSER2K'})
  if($ports.Count -ne 1){throw 'Expected FT232 COM7 not present'}
  $run=Join-Path $package $RunName
  if(Test-Path -LiteralPath $run){throw 'Physical procedure already attempted; no automatic retry'}
  New-Item -ItemType Directory -Path $run|Out-Null
  $gate=[ordered]@{task=$Task;utc=[DateTimeOffset]::UtcNow.ToString('o');baselineUtc=$GateUtc;maximumRequests=$maximum;retries=0;manifest=$ExpectedManifestHash;expectedB0=$ExpectedB0Hash}
  $gate|ConvertTo-Json|Set-Content -LiteralPath (Join-Path $run 'gate.json') -Encoding UTF8
  if($Task -eq 'Steady'){
   $args='--system-check --protocol-version 1 --port COM7 --baud 115200 --parity Even --timeout-ms 1000 --read-once --log "'+(Join-Path $run 'physical.jsonl')+'"'
  }else{$args+=' --execute --log "'+(Join-Path $run 'physical.jsonl')+'"';$deadline=280000}
 }
 $info=New-Object System.Diagnostics.ProcessStartInfo
 $info.FileName=Join-Path $package 'TR2.Supervision.Service.exe';$info.WorkingDirectory=$package
 $info.Arguments=$args;$info.UseShellExecute=$false;$info.RedirectStandardOutput=$true;$info.RedirectStandardError=$true
 $process=New-Object System.Diagnostics.Process;$process.StartInfo=$info
 if(-not $process.Start()){throw 'Process failed to start'}
 $stdout=$process.StandardOutput.ReadToEndAsync();$stderr=$process.StandardError.ReadToEndAsync()
 if(-not $process.WaitForExit($deadline)){$process.Kill();$process.WaitForExit();throw 'Bounded worker deadline; killed; no retry'}
 $code=$process.ExitCode
 $result=[ordered]@{task=$Task;utc=[DateTimeOffset]::UtcNow.ToString('o');package=$package;filesVerified=$files.Count;exitCode=$code;stdout=$stdout.Result;stderr=$stderr.Result;serialAuthorized=$physical}
 if($Task -eq 'Offline' -and $code -eq 0){
  if($stdout.Result -notmatch 'OFFLINE: no port opened'){throw 'No OFFLINE guarantee in result'}
  @{manifest=$ExpectedManifestHash;utc=[DateTimeOffset]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content -LiteralPath (Join-Path $package 'offline-success.json') -Encoding UTF8
 }
 if($physical){
  $result|ConvertTo-Json -Depth 4|Set-Content -LiteralPath (Join-Path $run 'result.json') -Encoding UTF8
  if(Test-Path -LiteralPath (Join-Path $run 'physical.jsonl')){$result['journalHash']=(Get-FileHash -LiteralPath (Join-Path $run 'physical.jsonl') -Algorithm SHA256).Hash}
 }
 $result|ConvertTo-Json -Depth 4;exit $code
}catch{[ordered]@{task=$Task;error=$_.Exception.Message;noAutomaticRetry=$true}|ConvertTo-Json;exit 1}
finally{if($process){$process.Dispose()};if($locked){$mutex.ReleaseMutex()};if($mutex){$mutex.Dispose()}}
