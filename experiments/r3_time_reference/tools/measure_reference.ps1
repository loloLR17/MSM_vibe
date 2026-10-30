param([switch]$Calibrate,[string]$Port,[int]$IntervalSeconds=10)
$ErrorActionPreference='Stop'
[Console]::OutputEncoding=[Text.UTF8Encoding]::new($false)
$frequency=[Diagnostics.Stopwatch]::Frequency
if(-not [Diagnostics.Stopwatch]::IsHighResolution -or $frequency -le 0){throw 'QPC unavailable'}
if(-not $Calibrate){
 $a=[Diagnostics.Stopwatch]::GetTimestamp();$u=[DateTime]::UtcNow
 Start-Sleep -Milliseconds 2000
 $b=[Diagnostics.Stopwatch]::GetTimestamp()
 @{mode='HOST_QPC_ONLY_NO_PORT_OPEN';high_resolution=$true;frequency=$frequency;ticks_before=$a;ticks_after=$b;delta_seconds=($b-$a)/[double]$frequency;UTC_before=$u.ToString('o');UTC_after=[DateTime]::UtcNow.ToString('o');does_not_qualify_target=$true} | ConvertTo-Json
 return
}
if($Port -notmatch '^COM[0-9]+$' -or $Port -eq 'COM4' -or $IntervalSeconds -lt 2 -or $IntervalSeconds -gt 60){throw 'Refused port/interval'}
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
$serial.ReadTimeout=2000;$serial.WriteTimeout=2000;$serial.DtrEnable=$false;$serial.RtsEnable=$false
function Query-Probe {
 $q=[byte[]]@(81);$buf=[byte[]]::new(32)
 $s=[Diagnostics.Stopwatch]::GetTimestamp();$serial.Write($q,0,1)
 $n=0
 while($n -lt 32){$n+=$serial.Read($buf,$n,32-$n)}
 $e=[Diagnostics.Stopwatch]::GetTimestamp()
 if([Text.Encoding]::ASCII.GetString($buf,0,4) -ne 'QPC1' -or [BitConverter]::ToUInt32($buf,4) -ne 0x001b0046 -or [BitConverter]::ToUInt32($buf,8) -ne 0x38425018 -or [BitConverter]::ToUInt32($buf,12) -ne 0x20333157){throw 'Wrong probe magic/UID'}
 # Full CRC/sequence/cycles are independently validated by Python after saving.
 return @{send_tick=$s;receive_tick=$e;frame_hex=([BitConverter]::ToString($buf)).Replace('-','').ToLowerInvariant()}
}
try{
 $serial.Open() # Exclusive OS handle; no forced close of any other owner.
 if($serial.BytesToRead){throw 'Unexpected pending data; do not silently discard'}
 $first=Query-Probe
 Start-Sleep -Seconds $IntervalSeconds
 $second=Query-Probe
 @{mode='SHORT_IDLE_PROBE';port=$Port;pnp_chain=$chain;high_resolution=$true;frequency=$frequency;queries=@($first,$second);UTC_annotation_only=[DateTime]::UtcNow.ToString('o');no_capture_duration_qualification=$true} | ConvertTo-Json -Depth 5
} finally {if($serial.IsOpen){$serial.Close()};$serial.Dispose()}
