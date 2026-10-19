@echo off
cd /d "%~dp0"
echo Effectuer STOP et attendre le terminal avant de fermer le service.
echo La demande ci-dessous ne sera executee que lorsque le prochain B3 confirme acquisition arretee.
echo shutdown> "%~dp0data\connection-control.shutdown"
pause
