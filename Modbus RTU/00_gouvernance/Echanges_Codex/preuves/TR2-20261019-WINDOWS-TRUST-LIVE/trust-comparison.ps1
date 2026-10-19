
$ErrorActionPreference='Continue'
$results=@()
foreach($path in @('Cert:\CurrentUser\My','Cert:\LocalMachine\My')) {
 try {$certs=@(Get-ChildItem $path -CodeSigningCert -ErrorAction Stop);$results += [pscustomobject]@{Store=$path;ReadSucceeded=$true;CodeSigningCount=$certs.Count;Certificates=@($certs | Select-Object Thumbprint,NotAfter,HasPrivateKey,Issuer)}} catch {$results += [pscustomobject]@{Store=$path;ReadSucceeded=$false;Error=$_.Exception.Message}}
}
$results | ConvertTo-Json -Depth 5
Get-ChildItem "$env:windir\System32\CodeIntegrity\CiPolicies\Active" -ErrorAction Continue | Select-Object Name,Length,LastWriteTimeUtc | ConvertTo-Json
Get-ItemProperty 'HKLM:\SYSTEM\CurrentControlSet\Control\CI\Policy' -ErrorAction Continue | Select-Object VerifiedAndReputablePolicyState | ConvertTo-Json
Get-Module -ListAvailable Az.Accounts,Az.CodeSigning,TrustedSigning,Microsoft.ArtifactSigning.Client | Select-Object Name,Version | ConvertTo-Json
[pscustomobject]@{AzureProfilePresent=(Test-Path "$env:USERPROFILE\.azure\azureProfile.json");AzureCliPresent=[bool](Get-Command az -ErrorAction SilentlyContinue);DeveloperSdkSignToolDir=(Test-Path "${env:ProgramFiles(x86)}\Windows Kits\10\bin")} | ConvertTo-Json
$paths=@('C:\Users\lolo\AppData\Local\MSM\TR2-r5b\TR2.Domain.dll','C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-DEMO-20261019\app\TR2.Domain.dll')
foreach($path in $paths) { $item=Get-Item -LiteralPath $path; $sign=Get-AuthenticodeSignature -LiteralPath $path; [pscustomobject]@{Path=$path;SHA256=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash;SignatureStatus=$sign.Status.ToString();SignerThumbprint=$sign.SignerCertificate.Thumbprint;Owner=(Get-Acl -LiteralPath $path).Owner;CreationTimeUtc=$item.CreationTimeUtc;LastWriteTimeUtc=$item.LastWriteTimeUtc;Streams=@(Get-Item -LiteralPath $path -Stream * | Select-Object Stream,Length)} | ConvertTo-Json -Depth 5; fsutil.exe file queryEA $path }
