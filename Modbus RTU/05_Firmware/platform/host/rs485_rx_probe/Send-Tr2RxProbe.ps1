# Offline by default. -Transmit is a physical action requiring separate authorization.
[CmdletBinding()]
param([switch]$Transmit)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-ModbusCrc([byte[]]$Bytes) {
    [int]$crc = 0xffff
    foreach ($b in $Bytes) {
        $crc = $crc -bxor $b
        for ($i = 0; $i -lt 8; $i++) {
            if ($crc -band 1) { $crc = ($crc -shr 1) -bxor 0xa001 }
            else { $crc = $crc -shr 1 }
        }
    }
    return $crc
}
# Unit 1 is a probe marker, not a claim about the unprovisioned device address.
# FC03, B1 register 1000, quantity 1, matching tr2_rtu_request 1 b1.
[byte[]]$body = 0x01,0x03,0x03,0xe8,0x00,0x01
$crc = Get-ModbusCrc $body
[byte[]]$frame = $body + [byte]($crc -band 255) + [byte]($crc -shr 8)
$hex = [BitConverter]::ToString($frame).Replace('-', ' ')
if ($hex -cne '01 03 03 E8 00 01 04 7A') { throw "CRC regression: $hex" }
if ((Get-ModbusCrc $frame) -ne 0) { throw 'Invalid complete-frame CRC' }
[byte[]]$bad = $frame.Clone()
$bad[7] = $bad[7] -bxor 1
if ((Get-ModbusCrc $bad) -eq 0) { throw 'Corrupted frame accepted' }
Write-Output "Request: $hex; COM7 115200 8E1; CRC/self-check OK"
if (-not $Transmit) { Write-Output 'OFFLINE: no serial port opened'; return }

$port = New-Object System.IO.Ports.SerialPort
$port.PortName = 'COM7'
$port.BaudRate = 115200
$port.DataBits = 8
$port.Parity = [System.IO.Ports.Parity]::Even
$port.StopBits = [System.IO.Ports.StopBits]::One
$port.Handshake = [System.IO.Ports.Handshake]::None
$port.DtrEnable = $false
$port.RtsEnable = $false
$port.WriteTimeout = 1000
$port.ReadTimeout = 100
$response = New-Object 'System.Collections.Generic.List[byte]'
try {
    $port.Open()
    Start-Sleep -Milliseconds 100
    if ($port.BytesToRead -ne 0) { throw 'Unexpected pre-existing RX bytes; stop without emission' }
    $port.Write($frame, 0, $frame.Length) # Single binary write; no newline, no retry.
    $clock = [Diagnostics.Stopwatch]::StartNew()
    while ($clock.ElapsedMilliseconds -lt 1000) {
        try { $response.Add([byte]$port.ReadByte()) }
        catch [TimeoutException] { }
    }
    if ($response.Count -eq 0) {
        Write-Output 'No response in ~1 s: expected possibility for RX-only firmware; inspect MCU counters'
    } else {
        Write-Output ('Observed PC RX (may be echo): ' + [BitConverter]::ToString($response.ToArray()))
    }
} finally {
    if ($port.IsOpen) { $port.Close() }
    $port.Dispose()
}
