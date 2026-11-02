$ErrorActionPreference='Stop';[Console]::OutputEncoding=[Text.UTF8Encoding]::new($false)
$d=@(Get-Disk|Where-Object {$_.Size -eq 511868665856}); if($d.Count -ne 1){throw 'Expected exactly SD512'}
$p=@(Get-Partition -DiskNumber $d[0].Number|Where-Object DriveLetter);if($p.Count -ne 1 -or $p[0].Offset -ne 1048576 -or $p[0].Size -ne 511867584512){throw 'Partition mismatch'}
$v=$p[0]|Get-Volume;if($v.FileSystem -ne 'FAT32' -or $v.AllocationUnitSize -ne 32768){throw 'Volume mismatch'}
$r=([string]$p[0].DriveLetter)+':\';$out='\\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\rearm-2H-C29\reported-red-recovery\PC-inventory'
$files=@(Get-ChildItem -LiteralPath $r -File|Select Name,Length);$candidates=@()
foreach($n in @('C290005.TR2','H2A0005.TR2','T100005.TR2')){
$f=Join-Path $r $n;if(-not(Test-Path -LiteralPath $f)){$candidates+=[pscustomobject]@{Name=$n;Exists=$false};continue}
$i=Get-Item -LiteralPath $f;$s=[IO.File]::Open($f,[IO.FileMode]::Open,[IO.FileAccess]::Read,[IO.FileShare]::Read)
try{if($i.Length -ge 512){$h=[byte[]]::new(512);if($s.Read($h,0,512) -ne 512){throw 'Header read short'};[IO.File]::WriteAllBytes((Join-Path $out ($n+'.header.bin')),$h);[void]$s.Seek(-512,[IO.SeekOrigin]::End);$t=[byte[]]::new(512);if($s.Read($t,0,512)-ne512){throw 'Tail read short'};[IO.File]::WriteAllBytes((Join-Path $out ($n+'.tail.bin')),$t)}}finally{$s.Dispose()}
$candidates+=[pscustomobject]@{Name=$n;Exists=$true;Length=$i.Length}
}
[pscustomobject]@{Disk=($d[0]|Select Number,FriendlyName,Size,PartitionStyle);Partition=($p[0]|Select DriveLetter,Offset,Size);Volume=($v|Select FileSystemLabel,FileSystem,AllocationUnitSize,SizeRemaining);Files=$files;Candidates=$candidates;ReadOnlySource=$true}|ConvertTo-Json -Depth 5
