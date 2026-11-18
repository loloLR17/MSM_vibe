param(
  [Parameter(Mandatory=$true)][ValidateSet('RX','RAW','MODBUS')][string]$Mode,
  [Parameter(Mandatory=$true)][string]$Port,
  [Parameter(Mandatory=$true)][string]$OutputPath,
  [Parameter(Mandatory=$true)][uint32]$Nonce,
  [switch]$PhysicalGateConfirmed
)
$ErrorActionPreference='Stop'
if(-not $PhysicalGateConfirmed){throw 'Physical preflight and sole-master ownership must be confirmed before this script.'}
if(Test-Path -LiteralPath $OutputPath){throw 'Evidence path exists; never overwrite a campaign.'}
$dev=@(Get-PnpDevice -PresentOnly | Where-Object { $_.Class -eq 'Ports' -and $_.FriendlyName -match ('\('+[regex]::Escape($Port)+'\)$') })
if($dev.Count -ne 1 -or $dev[0].Status -ne 'OK' -or $dev[0].InstanceId -notmatch 'VID_0403\+PID_6001\+BG04KSSWA'){throw 'Expected FTDI adapter identity is absent or ambiguous.'}
function Crc([byte[]]$Data){
  [uint32]$value=65535
  foreach($b in $Data){$value=$value -bxor $b;for($j=0;$j -lt 8;$j++){if($value -band 1){$value=($value -shr 1) -bxor 40961}else{$value=$value -shr 1}}}
  return [uint16]$value
}
function Frame([byte[]]$Body){
  $c=Crc $Body
  return [byte[]](@($Body)+@([byte]($c -band 255),[byte]($c -shr 8)))
}
function Hex([byte[]]$Data){return (($Data | ForEach-Object { $_.ToString('X2') }) -join ' ')}
$serial=New-Object System.IO.Ports.SerialPort($Port,115200,[System.IO.Ports.Parity]::Even,8,[System.IO.Ports.StopBits]::One)
$serial.Handshake=[System.IO.Ports.Handshake]::None
$serial.ReadTimeout=50;$serial.WriteTimeout=1000;$serial.DtrEnable=$false;$serial.RtsEnable=$false
$records=New-Object System.Collections.Generic.List[object]
$failed=$false;$failure=$null
try{
  $serial.Open() # SerialPort opens exclusively; never stop another owner silently.
  if($serial.BytesToRead){throw 'Unexpected pending bytes before first request; preserve and investigate.'}
  $count=if($Mode -eq 'RX'){1}else{8}
  for($seq=0;$seq -lt $count;$seq++){
    [byte[]]$body=if($Mode -eq 'MODBUS'){@(1,3,3,232,0,1)}else{@(72,49,56,63)+[BitConverter]::GetBytes($Nonce)+[BitConverter]::GetBytes([uint32]$seq)}
    [byte[]]$request=Frame $body
    $rx=New-Object System.Collections.Generic.List[byte]
    $timer=[System.Diagnostics.Stopwatch]::StartNew()
    $utc=[DateTime]::UtcNow.ToString('o')
    $serial.Write($request,0,$request.Length)
    $first=$null
    while($timer.ElapsedMilliseconds -lt 500){
      while($serial.BytesToRead){if($null -eq $first){$first=$timer.ElapsedTicks};$rx.Add([byte]$serial.ReadByte())}
      Start-Sleep -Milliseconds 1
    }
    [byte[]]$response=$rx.ToArray()
    $record=[ordered]@{sequence=$seq;utc=$utc;request_hex=(Hex $request);response_hex=(Hex $response);first_rx_ticks=$first;qpc_hz=[System.Diagnostics.Stopwatch]::Frequency;elapsed_ticks=$timer.ElapsedTicks;verdict='UNASSESSED'}
    if($Mode -eq 'RX'){
      if($response.Length -ne 0){$record.verdict='FAIL_UNEXPECTED_TX';$failed=$true}else{$record.verdict='SENT_RX_TARGET_COMPARISON_REQUIRED'}
    }elseif($Mode -eq 'RAW'){
      $ok=$response.Length -eq 26
      if($ok){$ok=(Hex $response[0..11]) -eq (Hex ([byte[]](@(84,49,56,33)+$body[4..11]))) -and (Crc $response) -eq 0}
      if($ok){$record.verdict='HOST_REPLY_VALID_TARGET_UID_COMPARISON_REQUIRED'}else{$record.verdict='FAIL_RAW_BYTES_CRC_OR_NONCE';$failed=$true}
    }else{
      [byte[]]$expected=Frame ([byte[]]@(1,3,2,0,0))
      if((Hex $response) -ceq (Hex $expected)){$record.verdict='PASS_HOST_MODBUS_UNKNOWN0'}else{$record.verdict='FAIL_MODBUS_RESPONSE';$failed=$true}
    }
    $records.Add($record)
    if($failed){break} # no retries, no automatic polarity changes
    Start-Sleep -Milliseconds 100
  }
}catch{$failed=$true;$failure=$_.Exception.Message}
finally{if($serial.IsOpen){$serial.Close()};$serial.Dispose()}
[ordered]@{mode=$Mode;port=$Port;adapter_instance=$dev[0].InstanceId;nonce=$Nonce;failed=$failed;failure=$failure;records=$records.ToArray();scope='Host wire observations only; target RAM/DE and gate evidence required separately'} | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $OutputPath -Encoding UTF8
if($failed){throw 'Campaign failed; evidence saved. Do not retry without diagnosis.'}
