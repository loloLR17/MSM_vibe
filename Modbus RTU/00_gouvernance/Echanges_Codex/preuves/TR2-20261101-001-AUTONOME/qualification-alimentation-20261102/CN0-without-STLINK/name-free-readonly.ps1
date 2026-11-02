$ErrorActionPreference='Stop';[Console]::OutputEncoding=[Text.UTF8Encoding]::new($false)
$d=@(Get-Disk|Where-Object {$_.Size -eq 511868665856});if($d.Count-ne1){throw 'SD512 identity absent/ambiguous'}
$p=@(Get-Partition -DiskNumber $d[0].Number|Where-Object DriveLetter);if($p.Count-ne1 -or $p[0].Offset-ne1048576 -or $p[0].Size-ne511867584512){throw 'Partition mismatch'};$v=$p[0]|Get-Volume;if($v.FileSystem-ne'FAT32' -or $v.AllocationUnitSize-ne32768){throw 'FAT32 mismatch'}
$root=([string]$p[0].DriveLetter)+':\';$path=Join-Path $root 'CN00005.TR2';if(Test-Path -LiteralPath $path){throw 'CN0 name already exists'};if($v.SizeRemaining-lt700000000){throw 'Insufficient free'}
[pscustomobject]@{Name='CN00005.TR2';Absent=$true;Root=$root;DiskBytes=$d[0].Size;PartitionOffset=$p[0].Offset;PartitionBytes=$p[0].Size;FileSystem=$v.FileSystem;ClusterBytes=$v.AllocationUnitSize;FreeBytes=$v.SizeRemaining;SourceReadOnly=$true}|ConvertTo-Json
