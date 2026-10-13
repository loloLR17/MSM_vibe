& {
param(
 [ValidateSet('Preflight','Baseline','Capture')][string]$Mode,
 [string]$RunId,[string]$ExpectedImageHash,[string]$GateUtc
)
$ErrorActionPreference='Stop';$ProgressPreference='SilentlyContinue'
[Console]::OutputEncoding=New-Object System.Text.UTF8Encoding($false)
$port=$null;$locked=$false;$mutex=$null;$journal=New-Object 'System.Collections.Generic.List[object]'
function Crc([byte[]]$data){
 [int]$value=65535
 foreach($b in $data){$value=$value -bxor $b;for($i=0;$i -lt 8;$i++){
  if($value -band 1){$value=($value -shr 1) -bxor 40961}else{$value=$value -shr 1}
 }}
 return $value
}
function Request([int]$address,[int]$quantity){
 [byte[]]$head=@(1,3,($address -shr 8),($address -band 255),($quantity -shr 8),($quantity -band 255))
 $c=Crc $head
 return ,([byte[]]($head+@(($c -band 255),($c -shr 8))))
}
$runDir=$null;$success=$false;$errorText=$null
try{
 if($env:COMPUTERNAME -ne 'LAPTOP-3F6J7STJ'){throw 'Unexpected HP identity'}
 if($RunId -notmatch '^A0402-[0-9TZ]+$'){throw 'Invalid unique run ID'}
 if($ExpectedImageHash -notmatch '^[a-f0-9]{64}$'){throw 'Trusted firmware SHA required'}
 if((Crc ([byte[]]@(1,3,3,232,0,1))) -ne 31236){throw 'CRC self-check refused'}
 $devices=@(Get-CimInstance Win32_PnPEntity|Where-Object{$_.Name -match '\(COM7\)' -and $_.Status -eq 'OK' -and $_.Service -eq 'FTSER2K'})
 if($devices.Count -ne 1){throw 'Expected FT232 COM7 not present'}
 $candidates=@(Get-Process|Where-Object{$_.ProcessName -match 'TR2|dotnet|python|putty|terminal|modbus'})
 if($candidates.Count){throw 'Potential serial master already active'}
 if($Mode -eq 'Preflight'){
  [ordered]@{mode=$Mode;hostname=$env:COMPUTERNAME;utc=[DateTimeOffset]::UtcNow.ToString('o');serialOpened=$false;crcSelfCheck=$true;port=$devices[0].Name;image=$ExpectedImageHash}|ConvertTo-Json -Depth 5
  exit 0
 }
 $age=([DateTimeOffset]::UtcNow-[DateTimeOffset]::Parse($GateUtc)).TotalSeconds
 if($age -lt 0 -or $age -gt 30){throw 'Stale MCU gate; no port opened'}
 $root=Join-Path $env:LOCALAPPDATA ('TR2\'+$RunId)
 $runDir=Join-Path $root $Mode
 if(Test-Path -LiteralPath $runDir){throw 'Phase already attempted; no automatic retry'}
 New-Item -ItemType Directory -Path $runDir -Force|Out-Null
 $mutex=New-Object System.Threading.Mutex($false,'Global\TR2-COM7-Orchestration')
 try{$locked=$mutex.WaitOne(0)}catch [System.Threading.AbandonedMutexException]{throw 'Abandoned serial orchestration'}
 if(-not $locked){throw 'Another COM7 orchestration active'}
 $port=New-Object System.IO.Ports.SerialPort('COM7',115200,[System.IO.Ports.Parity]::Even,8,[System.IO.Ports.StopBits]::One)
 $port.ReadTimeout=1000;$port.WriteTimeout=1000;$port.Handshake=[System.IO.Ports.Handshake]::None
 $port.Open()
 if($port.BytesToRead){throw 'Unsolicited data before first request'}
 $requests=if($Mode -eq 'Baseline'){@(@{name='B5-before';address=5000;quantity=20;delay=0;exception=0})}else{@(
  @{name='B0-trigger';address=0;quantity=21;delay=0;exception=0},
  @{name='B1-active';address=1000;quantity=20;delay=180;exception=0},
  @{name='B3-unavailable';address=3000;quantity=48;delay=180;exception=4},
  @{name='B5-denied';address=5000;quantity=20;delay=180;exception=4},
  @{name='B5-after';address=5000;quantity=20;delay=2200;exception=0})}
 foreach($q in $requests){
  if($q.delay){Start-Sleep -Milliseconds $q.delay}
  if($port.BytesToRead){throw 'Unexpected pending serial data; no new request'}
  [byte[]]$tx=Request $q.address $q.quantity
  $record=[ordered]@{name=$q.name;utc=[DateTimeOffset]::UtcNow.ToString('o');address=$q.address;quantity=$q.quantity;request=[BitConverter]::ToString($tx);response='';expectedException=$q.exception;crcValid=$false;passed=$false}
  $journal.Add($record)
  $watch=[Diagnostics.Stopwatch]::StartNew();$port.Write($tx,0,$tx.Length)
  $rx=New-Object 'System.Collections.Generic.List[byte]'
  $length=0
  while(-not $length -or $rx.Count -lt $length){
   if($watch.ElapsedMilliseconds -gt 1500){throw 'Bounded response deadline exceeded; no retry'}
   $rx.Add([byte]$port.ReadByte());$record.response=[BitConverter]::ToString($rx.ToArray())
   if($rx.Count -eq 3){
    if($rx[0] -ne 1){throw 'Unexpected slave ID'}
    if($rx[1] -eq 131){$length=5}
    elseif($rx[1] -eq 3){$length=5+[int]$rx[2]}
    else{throw 'Unexpected function code'}
   }
   if($rx.Count -gt 256){throw 'Response exceeds Modbus bound'}
  }
  $record.elapsedMs=$watch.Elapsed.TotalMilliseconds
  [byte[]]$raw=$rx.ToArray();[byte[]]$body=$raw[0..($raw.Length-3)]
  if((Crc $body) -ne ([int]$raw[$raw.Length-2]+256*[int]$raw[$raw.Length-1])){throw 'Response CRC mismatch'}
  $record.crcValid=$true
  if($q.exception){if($raw[1] -ne 131 -or $raw[2] -ne $q.exception){throw 'Expected bounded refusal missing'}}
  elseif($raw[1] -ne 3 -or $raw[2] -ne 2*$q.quantity){throw 'Expected authoritative read missing'}
  $record.passed=$true
 }
 $success=$true
}catch{$errorText=$_.Exception.Message}
finally{
 if($port){if($port.IsOpen){$port.Close()};$port.Dispose()}
 if($locked){$mutex.ReleaseMutex()};if($mutex){$mutex.Dispose()}
 if($Mode -ne 'Preflight'){
  $result=[ordered]@{mode=$Mode;runId=$RunId;image=$ExpectedImageHash;hostname=$env:COMPUTERNAME;utc=[DateTimeOffset]::UtcNow.ToString('o');gateUtc=$GateUtc;passed=$success;error=$errorText;requests=$journal.ToArray();maximumRequests=if($Mode -eq 'Baseline'){1}else{5};retries=0;serialClosed=$true}
  $json=$result|ConvertTo-Json -Depth 8
  if($runDir){[IO.File]::WriteAllText((Join-Path $runDir 'result.json'),$json,(New-Object Text.UTF8Encoding($false)))}
  $json
 }
}
if(-not $success){exit 1}

} -Mode 'Preflight' -RunId 'A0402-20261013T152741Z' -ExpectedImageHash 'b4301d6f3325c266edc19297413fd0885828e5625ec030a8acad340444b3fd9b'