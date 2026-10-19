@echo off
cd /d "%~dp0"
echo Preparation de STOP - partage exclusif de COM4 avec le service.
echo Attendre le terminal ET l'effet observe. L'acceptation seule ne suffit pas.
app\TR2.Supervision.Service.exe --live-command stop --port COM4 --control "%~dp0data\connection-control" --state "%~dp0data\transactions.json"
echo Code retour : %ERRORLEVEL%
pause
