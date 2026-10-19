from pathlib import Path
import subprocess,json,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-live-com4-loss-diag/physical');name='upgraded-stop'
cmd=['/mnt/c/Windows/System32/cmd.exe','/d','/c',r'C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-DEMO-20261019\STOP.cmd'];(p/(name+'-command.json')).write_text(json.dumps(dict(command=cmd,actual_installed_redirect=True,utc=datetime.datetime.now(datetime.timezone.utc).isoformat()),indent=2))
with (p/(name+'-raw.log')).open('wb') as f:z=subprocess.run(cmd,stdin=subprocess.DEVNULL,stdout=f,stderr=subprocess.STDOUT,timeout=230)
rows=[x for x in (p/(name+'-raw.log')).read_text(errors='replace').splitlines() if x.startswith('{')];(p/(name+'.jsonl')).write_text('\n'.join(rows)+'\n');a=[json.loads(x) for x in rows];terminal=[x for x in a if x.get('state')=='terminal_and_effect_observed'];assert len(terminal)==1 and terminal[0]['command']=='stop' and terminal[0]['transaction']==21
(p/(name+'-exit.json')).write_text(json.dumps(dict(exit=z.returncode,terminal=terminal[0],utc=datetime.datetime.now(datetime.timezone.utc).isoformat()),indent=2));print('Installed STOP launcher terminal5/21',z.returncode)
