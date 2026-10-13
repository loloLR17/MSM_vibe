$ErrorActionPreference='Stop';$ProgressPreference='SilentlyContinue';[Console]::OutputEncoding=New-Object System.Text.UTF8Encoding($false)
if($env:COMPUTERNAME -ne 'LAPTOP-3F6J7STJ'){throw 'Unexpected HP'}
$out=@()
foreach($mode in @('Baseline','Capture')){
 $file=Join-Path $env:LOCALAPPDATA ('TR2\A0402-20261013T152741Z\'+$mode+'\result.json')
 $bytes=[IO.File]::ReadAllBytes($file)
 $out+=[ordered]@{mode=$mode;bytes=$bytes.Length;sha256=(Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash;base64=[Convert]::ToBase64String($bytes)}
}
ConvertTo-Json -InputObject $out -Depth 4
