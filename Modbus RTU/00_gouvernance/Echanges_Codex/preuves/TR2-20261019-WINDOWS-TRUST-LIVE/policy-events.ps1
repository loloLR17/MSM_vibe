
$ErrorActionPreference='Continue'
$log='Microsoft-Windows-CodeIntegrity/Operational'
$result=@()
$events=Get-WinEvent -FilterHashtable @{LogName=$log;Id=3099} -MaxEvents 40 -ErrorAction SilentlyContinue
foreach($e in $events) { $xml=[xml]$e.ToXml();$data=@{};foreach($node in $xml.Event.EventData.Data){$data[$node.Name]=$node.'#text'};$result += [pscustomobject]@{Id=$e.Id;TimeCreatedUtc=$e.TimeCreated.ToUniversalTime().ToString('o');RecordId=$e.RecordId;Data=$data;Message=$e.Message} }
$result | ConvertTo-Json -Depth 7
$selected=@()
$events=Get-WinEvent -FilterHashtable @{LogName=$log;Id=3033,3077,3089,3090,3091,3092;StartTime=(Get-Date).AddDays(-3)} -MaxEvents 6000 -ErrorAction SilentlyContinue
foreach($e in $events) { if($e.Message -match 'TR2-r5b|TR2-LIVE-DEMO|TR2.Domain.dll') { $selected += [pscustomobject]@{Id=$e.Id;TimeCreatedUtc=$e.TimeCreated.ToUniversalTime().ToString('o');RecordId=$e.RecordId;Xml=$e.ToXml();Message=$e.Message} } }
$selected | ConvertTo-Json -Depth 5
