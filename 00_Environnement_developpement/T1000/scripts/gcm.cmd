@echo off
setlocal
set "PATH=C:\Users\lolo\AppData\Local\T1000\MinGit-2.56.0.2\cmd;C:\Users\lolo\AppData\Local\T1000\MinGit-2.56.0.2\mingw64\bin;%PATH%"
set "GCM_CREDENTIAL_STORE=wincredman"
set "GCM_PROVIDER=generic"
set "GCM_INTERACTIVE=never"
"C:\Users\lolo\AppData\Local\T1000\GCM-3.0.1\git-credential-manager.exe" %*
