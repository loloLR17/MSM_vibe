$ErrorActionPreference='Stop'
[Console]::OutputEncoding=[Text.UTF8Encoding]::new($false)
$taskDisks=@(Get-Disk | Where-Object {$_.Size -eq 511868665856})
if($taskDisks.Count -ne 1){throw 'Expected exactly one observed SD512 by exact capacity'}
$taskDisk=$taskDisks[0]
if($taskDisk.PartitionStyle -ne 'MBR'){throw 'Expected observed MBR card'}
$taskParts=@(Get-Partition -DiskNumber $taskDisk.Number | Where-Object {$_.DriveLetter})
if($taskParts.Count -ne 1){throw 'Expected one accessible partition'}
$taskPart=$taskParts[0]
if($taskPart.Offset -ne 1048576 -or $taskPart.Size -ne (999741376L*512L)){throw 'Partition differs from STM32 MBR'}
$taskVolume=$taskPart | Get-Volume
if($taskVolume.FileSystem -ne 'FAT32' -or $taskVolume.AllocationUnitSize -ne 32768){throw 'Native volume FAT32/cluster mismatch'}
$taskRoot=([string]$taskPart.DriveLetter)+':\'
$taskOut='\\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\native-pc-files-C'
if(Test-Path -LiteralPath $taskOut){throw 'Destination exists; do not overwrite prior extraction'}
$taskSpecs=@(@{Name='A1C0005.TR2';Length=2097152L;Duration=60000000L;Source='c885e50588b48105d2e12853cda0a9f06f1eb10e'})
# Validate source identity before creating any output; do not read another card.
foreach($taskSpec in $taskSpecs){
 $taskSource=Join-Path $taskRoot $taskSpec.Name
 $taskFile=Get-Item -LiteralPath $taskSource
 if($taskFile.Length -ne $taskSpec.Length){throw 'Preallocation length mismatch'}
 $taskHeader=[byte[]]::new(512)
 $taskStream=[IO.File]::Open($taskSource,[IO.FileMode]::Open,[IO.FileAccess]::Read,[IO.FileShare]::Read)
 try{if($taskStream.Read($taskHeader,0,512) -ne 512){throw 'Header read incomplete'}}finally{$taskStream.Dispose()}
 $taskCid=[BitConverter]::ToString($taskHeader[36..51]).Replace('-','').ToLowerInvariant()
 if([Text.Encoding]::ASCII.GetString($taskHeader,0,8) -ne 'TR2DUR01' -or $taskCid -ne '03534453443531328562e43cf80195eb'){throw 'Archive identity mismatch'}
 if([BitConverter]::ToUInt32($taskHeader,8) -ne 3 -or [BitConverter]::ToUInt32($taskHeader,124) -ne 16 -or [BitConverter]::ToUInt32($taskHeader,128) -ne 488){throw 'Archive profile mismatch'}
 if([BitConverter]::ToUInt64($taskHeader,168) -ne $taskSpec.Duration -or [Text.Encoding]::ASCII.GetString($taskHeader,52,40) -ne $taskSpec.Source){throw 'Archive source/duration mismatch'}
}
foreach($taskOld in @(@{Name='A1A0005.TR2';Hash='1370B795A465F1C1DF07A3FDECA9E16B1DF967483534F94A43D26A9E27DC761E'},@{Name='A1B0005.TR2';Hash='9BB593F8ED83E972C685B2E49411DF6429C1016659DDFEECA1051259B8139A16'})){if((Get-FileHash -LiteralPath (Join-Path $taskRoot $taskOld.Name) -Algorithm SHA256).Hash -ne $taskOld.Hash){throw 'Previous capture content changed'}}
[IO.Directory]::CreateDirectory($taskOut) | Out-Null
$taskResults=@()
foreach($taskSpec in $taskSpecs){
 $taskSource=Join-Path $taskRoot $taskSpec.Name
 $taskDest=Join-Path $taskOut $taskSpec.Name
 $taskSourceHash=(Get-FileHash -LiteralPath $taskSource -Algorithm SHA256).Hash
 Copy-Item -LiteralPath $taskSource -Destination $taskDest
 $taskDestHash=(Get-FileHash -LiteralPath $taskDest -Algorithm SHA256).Hash
 if($taskSourceHash -ne $taskDestHash -or (Get-Item -LiteralPath $taskDest).Length -ne $taskSpec.Length){throw 'Full copy/hash mismatch'}
 $taskResults+=[pscustomobject]@{Name=$taskSpec.Name;Length=$taskSpec.Length;Source=$taskSource;SourceSHA256=$taskSourceHash;Destination=$taskDest;DestinationSHA256=$taskDestHash}
}
[pscustomobject]@{Utc=[DateTime]::UtcNow.ToString('o');Disk=($taskDisk|Select Number,FriendlyName,SerialNumber,Size,PartitionStyle,BusType);Partition=($taskPart|Select PartitionNumber,DriveLetter,Offset,Size);Volume=($taskVolume|Select DriveLetter,FileSystemLabel,FileSystem,Size,SizeRemaining,AllocationUnitSize);Files=$taskResults;CardOperations='File reads only; no formatting, repair or source-file writes'}|ConvertTo-Json -Depth 6
