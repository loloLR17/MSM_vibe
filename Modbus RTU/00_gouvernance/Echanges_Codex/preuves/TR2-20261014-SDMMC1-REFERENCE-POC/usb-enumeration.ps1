$ErrorActionPreference = 'Stop'
$benchDevices = @(Get-PnpDevice -PresentOnly | Where-Object { $_.InstanceId -match '^USB\\VID_(0483&PID_572B|0000)' } | Select-Object Status, Class, FriendlyName, InstanceId)
$benchDisks = @(Get-Disk | Where-Object { $_.FriendlyName -match 'STM32.*SD|SD.*Card' -and $_.BusType -eq 'USB' } | Select-Object Number, FriendlyName, SerialNumber, Size, BusType, OperationalStatus, PartitionStyle)
[ordered]@{utc=[DateTime]::UtcNow.ToString('o'); expected='USB VID0483 PID572B, firmware descriptor';devices=$benchDevices;disks=$benchDisks} | ConvertTo-Json -Depth 5
