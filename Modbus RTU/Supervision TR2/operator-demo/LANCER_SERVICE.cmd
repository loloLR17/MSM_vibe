@echo off
cd /d "%~dp0"
if not exist data mkdir data
set TR2_SESSION=%RANDOM%-%RANDOM%
echo TR2 LIVE - service Windows, COM4 exclusif, sans limite de duree.
echo Ouvrir http://127.0.0.1:5100/live-device.html?deviceId=1^&view=vibration
echo START et STOP : utiliser les deux lanceurs. STOP peut prendre 30-60 secondes.
echo Ne pas fermer ce service pendant acquisition. Journaux dans data.
app\TR2.Supervision.Service.exe --live-direct COM4 --allow-diagnostic --database "%~dp0data\live.db" --listen http://127.0.0.1:5100 --trace "%~dp0data\serial-%TR2_SESSION%.jsonl" --seconds 0 --unit 1 --connection-control "%~dp0data\connection-control" > "%~dp0data\service-%TR2_SESSION%.log" 2>&1
echo Service termine. Consulter le journal en cas d'erreur.
pause
