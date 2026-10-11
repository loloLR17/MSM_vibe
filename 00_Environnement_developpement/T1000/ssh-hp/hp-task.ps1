param(
    [ValidateSet('Probe','Inventory','Offline','FailureProbe')][string]$Task = 'Probe',
    [string]$ExePath,
    [string]$ExpectedSha256
)
$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'
[Console]::OutputEncoding = New-Object System.Text.UTF8Encoding($false)
try {
    if ($env:COMPUTERNAME -ne 'LAPTOP-3F6J7STJ') { throw 'Unexpected HP identity' }
    if ($Task -eq 'FailureProbe') { Write-Output 'EXPECTED_FAILURE_PROBE'; exit 17 }
    if ($Task -eq 'Probe') {
        [ordered]@{hostname=$env:COMPUTERNAME; cwd=(Get-Location).Path; powershell=$PSVersionTable.PSVersion.ToString(); administrator=([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)} | ConvertTo-Json
        exit 0
    }
    if ($Task -eq 'Inventory') {
        $artifacts = @()
        foreach ($path in @((Join-Path $env:USERPROFILE 'Desktop'), (Join-Path $env:USERPROFILE 'Documents'), (Join-Path $env:USERPROFILE 'Downloads'), (Join-Path $env:LOCALAPPDATA 'TR2'))) {
            if (Test-Path -LiteralPath $path) {
                $artifacts += @(Get-ChildItem -LiteralPath $path -File -Recurse -Depth 4 -ErrorAction SilentlyContinue | Where-Object { $_.Name -in @('TR2.Supervision.Service.exe','M014-A-package-manifest.json','M014-D-package-manifest.json','TR2.Supervision.sln') } | Select-Object FullName,Length)
            }
        }
        $dotnet = Get-Command dotnet -ErrorAction SilentlyContinue
        $runtimes = if ($dotnet) { @(& $dotnet.Source --list-runtimes) } else { @() }
        $config = Join-Path $env:ProgramData 'ssh\sshd_config'
        $policy = if (Test-Path -LiteralPath $config) { @(Get-Content -LiteralPath $config | Where-Object {$_ -match '^\s*(PasswordAuthentication|PubkeyAuthentication|PermitRootLogin|AllowUsers|AllowGroups|Match|AuthorizedKeysFile)\s'} | ForEach-Object {[string]$_}) } else { @() }
        $sshdPath = Join-Path $env:WINDIR 'System32\OpenSSH\sshd.exe'
        $effective = if (Test-Path -LiteralPath $sshdPath) { @(& $sshdPath -T | Where-Object {$_ -match '^(passwordauthentication|pubkeyauthentication|permitrootlogin|authorizedkeysfile|allowusers|allowgroups) '} | ForEach-Object {[string]$_}); if ($LASTEXITCODE -ne 0) { throw 'Cannot inspect effective sshd policy' } } else { @() }
        [ordered]@{hostname=$env:COMPUTERNAME; dotnetPath=if($dotnet){$dotnet.Source}else{$null}; runtimes=$runtimes; ports=@([System.IO.Ports.SerialPort]::GetPortNames()); com7=@(Get-CimInstance Win32_PnPEntity | Where-Object {$_.Name -match '\(COM7\)'} | Select-Object Name,Status,Service); candidates=@(Get-Process | Where-Object {$_.ProcessName -match 'TR2|dotnet|python|putty|terminal|modbus'} | Select-Object Id,ProcessName); artifacts=$artifacts; sshd=(Get-Service sshd | Select-Object Status,StartType); sshPolicy=$policy; sshEffectiveGlobalPolicy=$effective; portOwnership='NOT PROVEN: process inventory does not enumerate every kernel handle; no serial port opened'} | ConvertTo-Json -Depth 6
        exit 0
    }
    # OFFLINE only. There is deliberately no physical/ReadOnce task or arbitrary argv.
    if ($ExpectedSha256 -notmatch '^[a-fA-F0-9]{64}$') { throw 'A trusted executable SHA256 is mandatory' }
    $resolved = (Resolve-Path -LiteralPath $ExePath).Path
    if ((Get-FileHash -LiteralPath $resolved -Algorithm SHA256).Hash -ne $ExpectedSha256) { throw 'Executable hash mismatch' }
    $package = Split-Path -Parent $resolved
    if ($ExpectedSha256 -eq '13228dfe3d30cf01829c16be1be22f30b061cc0cb0916b2e6a499b8fd1f96872') {
        $manifest = Join-Path $package 'M014-A-package-manifest.json'
        $manifestHash = 'eeaf70a35f81a9d9a5102cef011dc54bb4e9eae2b774e75af75689b1dc6db7a3'
    } elseif ($ExpectedSha256 -eq '9b5d247be3f86ef606352f3f8a9faa74cacd5cae9a39cb314f6e93edf62bf6d6') {
        $manifest = Join-Path $package 'M014-D-package-manifest.json'
        $manifestHash = 'a3d60750443517ab3238de3c759ef3f04c6dee322ee2d96bb9e21c20dc72577c'
    } else { throw 'Only reviewed M014-A/D packages are supported' }
    if ((Get-FileHash -LiteralPath $manifest -Algorithm SHA256).Hash -ne $manifestHash) { throw 'Manifest hash mismatch' }
    $files = @((Get-Content -LiteralPath $manifest -Raw | ConvertFrom-Json).files)
    if ($files.Count -ne 370) { throw 'Unexpected manifest file count' }
    foreach ($file in $files) {
        $candidate = [IO.Path]::GetFullPath((Join-Path $package $file.path))
        $prefix = [IO.Path]::GetFullPath($package).TrimEnd('\') + '\'
        if ([IO.Path]::IsPathRooted($file.path) -or -not $candidate.StartsWith($prefix, [StringComparison]::OrdinalIgnoreCase)) { throw 'Unsafe manifest path' }
        $item = Get-Item -LiteralPath $candidate
        if ($item.Length -ne $file.bytes -or (Get-FileHash -LiteralPath $item.FullName -Algorithm SHA256).Hash -ne $file.sha256) { throw 'Package file mismatch' }
    }
    $mutex = New-Object System.Threading.Mutex($false, 'Global\TR2-COM7-Orchestration')
    $locked = $false
    try {
        try { $locked = $mutex.WaitOne(0) } catch [System.Threading.AbandonedMutexException] { $locked = $true }
        if (-not $locked) { throw 'Another orchestration holds the COM7 lock' }
        $candidates = @(Get-Process | Where-Object {$_.ProcessName -match 'TR2|dotnet|python|putty|terminal|modbus'})
        if ($candidates.Count -gt 0) { throw 'Potential serial master already running; refusing launch' }
        $info = New-Object System.Diagnostics.ProcessStartInfo
        $info.FileName = $resolved
        $info.WorkingDirectory = Split-Path -Parent $resolved
        $info.Arguments = '--diagnostic --port COM7 --baud 115200 --parity Even --timeout-ms 1000'
        $info.UseShellExecute = $false
        $info.RedirectStandardOutput = $true
        $info.RedirectStandardError = $true
        $process = New-Object System.Diagnostics.Process
        $process.StartInfo = $info
        try {
            if (-not $process.Start()) { throw 'Process failed to start' }
            $stdout = $process.StandardOutput.ReadToEndAsync()
            $stderr = $process.StandardError.ReadToEndAsync()
            if (-not $process.WaitForExit(15000)) { $process.Kill(); $process.WaitForExit(); throw 'OFFLINE process timed out; killed, no automatic retry' }
            $code = $process.ExitCode
            [ordered]@{hostname=$env:COMPUTERNAME; task='Offline'; manifestFilesVerified=$files.Count; executable=$resolved; sha256=$ExpectedSha256; exitCode=$code; stdout=$stdout.Result; stderr=$stderr.Result; serialOpened=$false; transmissionAuthorized=$false} | ConvertTo-Json -Depth 4
            exit $code
        } finally { $process.Dispose() }
    } finally { if ($locked) { $mutex.ReleaseMutex() }; $mutex.Dispose() }
} catch { [ordered]@{task=$Task; error=$_.Exception.Message; transmissionAuthorized=$false} | ConvertTo-Json; exit 1 }
