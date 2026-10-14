$ErrorActionPreference = 'Stop'
$phoneDevices = @(Get-PnpDevice -PresentOnly | Where-Object { $_.InstanceId -match '^USB\\VID_04E8' -or ($_.Class -eq 'WPD' -and $_.FriendlyName -match 'Galaxy|Samsung') } | Select-Object Status, Class, FriendlyName, InstanceId)
[ordered]@{utc=[DateTime]::UtcNow.ToString('o'); device_test='Galaxy on same cable and same host port, user confirmed';devices=$phoneDevices} | ConvertTo-Json -Depth 5
