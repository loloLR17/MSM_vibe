from pathlib import Path
import subprocess,json,datetime,sys
p=Path('/home/lolo/dev/msm/reports/TR2-live-com4-loss-diag');w=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-LIVE-DEMO-20261019');n=sys.argv[1];before={x.name for x in (w/'data').iterdir()};
cmd=['/mnt/c/Windows/System32/cmd.exe','/d','/c',r'C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-DEMO-20261019\LANCER_SERVICE.cmd']
(p/(n+'-launcher.json')).write_text(json.dumps(dict(command=cmd,utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),existing_files=sorted(before)),indent=2)+'\n')
with (p/(n+'-launcher.log')).open('wb') as log:
 z=subprocess.Popen(cmd,stdin=subprocess.DEVNULL,stdout=log,stderr=subprocess.STDOUT)
 print('Native launcher started',z.pid,flush=True);rc=z.wait()
(p/(n+'-launcher-exit.json')).write_text(json.dumps(dict(exit=rc,utc=datetime.datetime.now(datetime.timezone.utc).isoformat())))
print('Native launcher exit',rc,flush=True)
