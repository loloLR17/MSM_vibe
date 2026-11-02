$ErrorActionPreference='Stop'
[Console]::OutputEncoding=[Text.UTF8Encoding]::new($false)
$qd=@(Get-Disk | Where-Object {$_.Size -eq 511868665856})
if($qd.Count -ne 1 -or $qd[0].PartitionStyle -ne 'MBR'){throw 'SD512 identity/partition mismatch'}
$qp=@(Get-Partition -DiskNumber $qd[0].Number | Where-Object {$_.DriveLetter})
if($qp.Count -ne 1 -or $qp[0].Offset -ne 1048576 -or $qp[0].Size -ne 511867584512){throw 'Partition mismatch'}
$qv=$qp[0] | Get-Volume
if($qv.FileSystem -ne 'FAT32' -or $qv.AllocationUnitSize -ne 32768){throw 'FAT32 profile mismatch'}
$qr=([string]$qp[0].DriveLetter)+':\'
$qh=[byte[]]::new(512);$qs=[IO.File]::OpenRead((Join-Path $qr 'A1H0005.TR2'))
try{if($qs.Read($qh,0,512) -ne 512){throw 'Header read incomplete'}}finally{$qs.Dispose()}
if([BitConverter]::ToString($qh[36..51]).Replace('-','').ToLowerInvariant() -ne '03534453443531328562e43cf80195eb'){throw 'Card archive CID mismatch'}
$qfiles=@();foreach($qn in @('B010005.TR2','B020005.TR2')){$qe=Test-Path -LiteralPath (Join-Path $qr $qn);if($qe){throw 'New qualification name already exists'};$qfiles+=[pscustomobject]@{Name=$qn;Exists=$qe}}
[pscustomobject]@{Utc=[DateTime]::UtcNow.ToString('o');Disk=($qd[0]|Select Number,Size,PartitionStyle,FriendlyName);Volume=($qv|Select DriveLetter,FileSystem,AllocationUnitSize,SizeRemaining);NewFiles=$qfiles;Operations='Read only; no whole-card copy or source-file changes'}|ConvertTo-Json -Depth 5
