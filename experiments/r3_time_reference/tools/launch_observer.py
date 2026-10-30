"""Start an independent Windows clock instrument; never starts/stops the MCU.
No background Codex/WSL agent, no security-policy change, no encoded command.
The parent exits promptly; child evidence is on native Windows storage.
"""
from pathlib import Path
import argparse,subprocess,json,hashlib
PS='/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe'
def quote(s):return "'"+str(s).replace("'","''")+"'"
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--port',required=True);ap.add_argument('--output',required=True);ap.add_argument('--seconds',type=int,required=True);ap.add_argument('--run',type=int,choices=[3,5],required=True);a=ap.parse_args()
 if a.port=='COM4' or not a.port.startswith('COM') or not a.port[3:].isdigit():raise ValueError('Refused port')
 source=Path(__file__).with_name('observe_endurance.ps1').read_text()
 # Explicit, readable command transport already used by qualified1h witness.
 # Refuse Windows quoting ambiguities. Never -EncodedCommand/ExecutionPolicy.
 if '"' in source:raise ValueError('Unexpected quote requires explicit review')
 command='& {\n'+source+'\n} -Witness -Port '+a.port+' -OutputDirectory '+quote(a.output)+f' -MinimumSeconds {a.seconds} -ExpectedRun {a.run} -TerminalGraceSeconds {60 if a.run==3 else 3600}'
 arguments='-NoProfile -NonInteractive -Command "'+command+'"'
 guard="if((Test-Path -LiteralPath "+quote(a.output)+") -or (Test-Path -LiteralPath "+quote(a.output+'.stdout.log')+") -or (Test-Path -LiteralPath "+quote(a.output+'.stderr.log')+") -or (Test-Path -LiteralPath "+quote(a.output+'.actual.ps1')+")){throw 'Evidence collision'};"
 guard+="[IO.File]::WriteAllText("+quote(a.output+'.actual.ps1')+","+quote(command)+");"
 parent='[Console]::OutputEncoding=[Text.UTF8Encoding]::new($false);$ErrorActionPreference=\'Stop\';'+guard+'$p=Start-Process -FilePath '+quote(r'C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe')+' -ArgumentList '+quote(arguments)+' -WindowStyle Hidden -RedirectStandardOutput '+quote(a.output+'.stdout.log')+' -RedirectStandardError '+quote(a.output+'.stderr.log')+' -PassThru;$p|Select-Object Id,StartTime|ConvertTo-Json'
 r=subprocess.run([PS,'-NoProfile','-NonInteractive','-Command',parent],capture_output=True,timeout=20);r.check_returncode()
 print(json.dumps(dict(native_process=json.loads(r.stdout.decode('utf-8-sig')),observer_sha256=hashlib.sha256(source.encode()).hexdigest(),output=a.output,expected_run=a.run,minimum_QPC_s=a.seconds,parent_has_exited=True,STM32_control=False),indent=2))
if __name__=='__main__':main()
