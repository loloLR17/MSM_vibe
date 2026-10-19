[Console]::OutputEncoding=New-Object System.Text.UTF8Encoding($false)
$ErrorActionPreference='Stop'
$p='C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-DEMO-20261019\data\connection-control'
$token='disconnected:'+([guid]::NewGuid().ToString());$owns=$false;$s=$null
try {
$f=[IO.File]::Open($p,[IO.FileMode]::CreateNew,[IO.FileAccess]::Write,[IO.FileShare]::Read)
$w=[IO.StreamWriter]::new($f);$w.Write($token);$w.Flush();$f.Flush($true);$w.Dispose();$owns=$true
$until=(Get-Date).AddSeconds(15);$ack=$null
while((Get-Date) -lt $until){try{$ack=[IO.File]::ReadAllText($p+'.released')}catch{};if($ack -eq $token){break};Start-Sleep -Milliseconds 50}
if($ack -ne $token){throw 'NO MATCHING ACK - NO SERIAL OPEN'}
$s=[IO.Ports.SerialPort]::new('COM4',115200,[IO.Ports.Parity]::Even,8,[IO.Ports.StopBits]::One);$s.Open()
[pscustomobject]@{Utc=[datetime]::UtcNow.ToString('o');MatchingPauseAck=$true;ExclusiveCOM4Open=$s.IsOpen;Transmission=$false;Token=$token}|ConvertTo-Json
$s.Close();$s.Dispose();$s=$null
}finally{if($s){$s.Dispose()};if($owns -and [IO.File]::ReadAllText($p) -eq $token){[IO.File]::Delete($p)}}
