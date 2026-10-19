@echo off
cd /d "%~dp0"
echo Preparation de START - partage exclusif de COM4 avec le service.
echo Attendre le terminal ET l'effet observe. L'acceptation seule ne suffit pas.
app\TR2.Supervision.Service.exe --live-command start --port COM4 --control "%~dp0data\connection-control" --state "%~dp0data\transactions.json"
echo Code retour : %ERRORLEVEL%
pause
