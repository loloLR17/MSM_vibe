from pathlib import Path
import subprocess,json
P=Path(__file__).parent
s=(P/'native_host_snapshot.ps1').read_text()
(P/'host-snapshot-short.ps1').write_text(s)
r=subprocess.run(['/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe','-NoProfile','-NonInteractive','-Command',s],capture_output=True,timeout=25);(P/'native-host.json').write_bytes(r.stdout);(P/'host-snapshot-short.stderr').write_bytes(r.stderr);r.check_returncode()
v=json.loads(r.stdout.decode('utf-8-sig'));assert len(v['Parents'])==1 and '0032001F3235510C37333439' in v['Parents'][0]['Data']
print(json.dumps(dict(COM5_parent_verified=True,Power=v['Power'],scope='short two-second bench gate; no long-run power admission claimed'),indent=2))
