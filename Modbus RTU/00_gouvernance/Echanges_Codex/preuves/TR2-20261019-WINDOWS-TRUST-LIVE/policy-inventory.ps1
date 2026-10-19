
$ErrorActionPreference='Continue'
$identity=[Security.Principal.WindowsIdentity]::GetCurrent(); $principal=New-Object Security.Principal.WindowsPrincipal($identity)
[pscustomobject]@{IsAdministrator=$principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator);IsSystem=$identity.IsSystem;PowerShellVersion=$PSVersionTable.PSVersion.ToString()} | ConvertTo-Json
Get-ItemProperty 'HKLM:\SYSTEM\CurrentControlSet\Control\CI\Policy' -ErrorAction SilentlyContinue | Select-Object VerifiedAndReputablePolicyState | ConvertTo-Json
if(Test-Path "$env:windir\System32\CiTool.exe") { & "$env:windir\System32\CiTool.exe" -lp -json }
Get-CimInstance Win32_ComputerSystem | Select-Object PartOfDomain,DomainRole | ConvertTo-Json
Get-ChildItem Cert:\CurrentUser\My,Cert:\LocalMachine\My -CodeSigningCert -ErrorAction SilentlyContinue | Select-Object Subject,Issuer,Thumbprint,NotBefore,NotAfter,HasPrivateKey,@{N='Eku';E={($_.EnhancedKeyUsageList | ForEach-Object {$_.ObjectId.Value}) -join ','}} | ConvertTo-Json -Depth 4
Get-Command signtool.exe,az.cmd,az,AzureSignTool.exe -ErrorAction SilentlyContinue | Select-Object Name,Source | ConvertTo-Json
Get-ItemProperty 'HKLM:\SOFTWARE\Policies\Microsoft\Windows\SrpV2' -ErrorAction SilentlyContinue | Select-Object * -ExcludeProperty PSPath,PSParentPath,PSChildName,PSDrive,PSProvider | ConvertTo-Json -Depth 3
