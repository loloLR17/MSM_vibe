param([switch]$Witness,[string]$Port,[string]$OutputDirectory,[int]$MinimumSeconds=28801,[int]$ExpectedRun=5,[int]$TerminalGraceSeconds=3600)
if(-not $Witness){throw 'Explicit -Witness required; no port opened'}
if($MinimumSeconds -lt 1 -or $MinimumSeconds -gt 28801 -or $ExpectedRun -notin @(3,5)){throw 'Refused witness parameters'}
$ErrorActionPreference='Stop'
# Passive qualification instrument only: no START, STOP, flash, GDB or network.
if(-not $OutputDirectory -or -not [IO.Path]::IsPathRooted($OutputDirectory) -or $OutputDirectory.StartsWith('\\')){throw 'Native local Windows evidence directory required'}
if($TerminalGraceSeconds -lt 60 -or $TerminalGraceSeconds -gt 3600){throw 'Refused terminal grace'}
if([IO.Directory]::Exists($OutputDirectory)){throw 'Evidence directory exists; never overwrite'}
[IO.Directory]::CreateDirectory($OutputDirectory)|Out-Null
$journal=[IO.Path]::Combine($OutputDirectory,'queries.jsonl')
$ready=[IO.Path]::Combine($OutputDirectory,'ready.json')
$summary=[IO.Path]::Combine($OutputDirectory,'summary.json')
trap {if([IO.Directory]::Exists($OutputDirectory)){[IO.File]::WriteAllText([IO.Path]::Combine($OutputDirectory,'instrument-error.txt'),$_.Exception.Message)};exit 1}
[Console]::OutputEncoding=[Text.UTF8Encoding]::new($false)
$frequency=[Diagnostics.Stopwatch]::Frequency
if(-not [Diagnostics.Stopwatch]::IsHighResolution -or $frequency -le 0){throw 'QPC unavailable'}
if($Port -notmatch '^COM[0-9]+$' -or $Port -eq 'COM4'){throw 'Refused port/interval'}
$ports=@(Get-CimInstance Win32_SerialPort | Where-Object DeviceID -eq $Port)
if($ports.Count -ne 1){throw 'Expected unique serial port'}
$id=$ports[0].PNPDeviceID;$chain=@();$matched=$false
for($i=0;$i -lt 6 -and $id;$i++){
 $chain+=$id
 if($id -match '^USB\\VID_0483&PID_374E\\0032001F3235510C37333439$'){$matched=$true;break}
 $id=(Get-PnpDeviceProperty -InstanceId $id -KeyName 'DEVPKEY_Device_Parent').Data
}
if(-not $matched){throw 'Port is not VCP of expected TR2-002 STLINK SN'}
$serial=[IO.Ports.SerialPort]::new($Port,115200,[IO.Ports.Parity]::None,8,[IO.Ports.StopBits]::One)
$serial.ReadTimeout=120000;$serial.WriteTimeout=2000;$serial.DtrEnable=$false;$serial.RtsEnable=$false

function Crc32([byte[]]$b,[int]$n){
 [uint32]$c=[uint32]::MaxValue
 for($i=0;$i -lt $n;$i++){
  $c=$c -bxor [uint32]$b[$i]
  for($j=0;$j -lt 8;$j++){if($c -band 1){$c=[uint32](($c -shr 1) -bxor [uint32]3988292384)}else{$c=$c -shr 1}}
 }
 return [uint32]($c -bxor [uint32]::MaxValue)
}
function Query-Witness {
 $q=[byte[]]@(81);$buf=[byte[]]::new(48)
 $s=[Diagnostics.Stopwatch]::GetTimestamp();$serial.Write($q,0,1);$n=0
 while($n -lt 48){$n+=$serial.Read($buf,$n,48-$n)}
 $e=[Diagnostics.Stopwatch]::GetTimestamp()
 if([Text.Encoding]::ASCII.GetString($buf,0,4) -ne 'QPC2' -or [BitConverter]::ToUInt32($buf,4) -ne 0x001b0046 -or [BitConverter]::ToUInt32($buf,8) -ne 0x38425018 -or [BitConverter]::ToUInt32($buf,12) -ne 0x20333157 -or (Crc32 $buf 44) -ne [BitConverter]::ToUInt32($buf,44)){throw 'Wrong witness magic/UID/CRC'}
 $row=@{send_tick=$s;receive_tick=$e;flags=[BitConverter]::ToUInt32($buf,28);frame_hex=([BitConverter]::ToString($buf)).Replace('-','').ToLowerInvariant()}
 [IO.File]::AppendAllText($journal,($row|ConvertTo-Json -Compress)+[Environment]::NewLine)
 return $row
}
$queries=@();$first=$null;$second=$null;$terminal=$null;$failure=$null
try {
 $serial.Open();@{pid=$PID;frequency=$frequency;port=$Port;pnp_chain=$chain;UTC_annotation_only=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json -Depth 5|Set-Content -LiteralPath $ready -Encoding UTF8;if($serial.BytesToRead){throw 'Unexpected pending data; not discarded'}
 $deadline=[Diagnostics.Stopwatch]::GetTimestamp()+600*$frequency
 while(-not $first){
  $r=Query-Witness;$queries+=$r
  if(($r.flags -band 255) -eq 1 -and ($r.flags -shr 8) -eq $ExpectedRun){$first=$r;break}
  if(($r.flags -band 2) -or ((($r.flags -band 4) -ne 0) -and ($r.flags -shr 8) -eq $ExpectedRun)){throw 'Capture ended before witness'}
  if([Diagnostics.Stopwatch]::GetTimestamp() -gt $deadline){throw 'No active capture in600s'}
  Start-Sleep -Milliseconds 200
 }
 $serial.ReadTimeout=2000
 [long]$target=[long]$first.receive_tick+[long]$MinimumSeconds*[long]$frequency+2
 while([Diagnostics.Stopwatch]::GetTimestamp() -lt $target){Start-Sleep -Milliseconds 200}
 $second=Query-Witness;$queries+=$second
 if(($second.flags -band 255) -ne 1 -or ($second.flags -shr 8) -ne $ExpectedRun){throw 'Capture not active at second witness'}
 $finishLimit=[Diagnostics.Stopwatch]::GetTimestamp()+$TerminalGraceSeconds*$frequency
 while(-not $terminal){
  Start-Sleep -Seconds $(if($ExpectedRun -eq 5){10}else{1})
  $r=Query-Witness;$queries+=$r
  if(($r.flags -band 2) -ne 0){throw 'Terminal error'}
  if(($r.flags -band 255) -eq 4 -and ($r.flags -shr 8) -eq $ExpectedRun){$terminal=$r;break}
  if([Diagnostics.Stopwatch]::GetTimestamp() -gt $finishLimit){throw 'No successful terminal; do not assume capture stopped'}
 }
} catch {$failure=$_.Exception.Message}
finally {if($serial.IsOpen){$serial.Close()};$serial.Dispose()}
$result=@{instrument_version=3;autonomous_native_Windows_process=$true;no_STM32_control=$true;mode='ACTIVE_INNER_INTERVAL_QPC2';frequency=$frequency;high_resolution=$true;minimum_seconds=$MinimumSeconds;expected_run=$ExpectedRun;port=$Port;pnp_chain=$chain;first=$first;second=$second;terminal=$terminal;all_queries=$queries;failure=$failure;UTC_annotation_only=[DateTime]::UtcNow.ToString('o');does_not_stop_capture=$true}
$result|ConvertTo-Json -Depth 6|Set-Content -LiteralPath $summary -Encoding UTF8
$result|ConvertTo-Json -Depth 6
if($failure){throw $failure}
