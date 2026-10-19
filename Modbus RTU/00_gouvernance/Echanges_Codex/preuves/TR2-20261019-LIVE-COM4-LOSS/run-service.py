from pathlib import Path
import subprocess,json,datetime,time
p=Path('/home/lolo/dev/msm/reports/TR2-live-com4-loss-diag/physical');p.mkdir(exist_ok=True)
a=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-LIVE-DEMO-20261019');d=a/'data';d.mkdir(exist_ok=True)
win=lambda x:subprocess.check_output(['wslpath','-w',str(x)],text=True).strip()
n=__import__('sys').argv[1]
cmd=[str(a/'app/TR2.Supervision.Service.exe'),'--live-direct','COM4','--allow-diagnostic','--database',win(d/'live.db'),'--listen','http://127.0.0.1:5100','--trace',win(d/('serial-'+n+'.jsonl')),'--seconds','0','--unit','1','--connection-control',win(d/'connection-control')]
assert not (d/'connection-control').exists();assert not (d/'connection-control.shutdown').exists()
(p/('service-'+n+'-command.json')).write_text(json.dumps(dict(command=cmd,utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),app_source='c36c971',no_security_changes=True),indent=2))
with (p/('service-'+n+'.log')).open('wb') as log:
 proc=subprocess.Popen(cmd,stdout=log,stderr=subprocess.STDOUT)
 (p/'service-pid.json').write_text(json.dumps(dict(pid=proc.pid,run=n)))
 result=proc.wait()
(p/('service-'+n+'-exit.json')).write_text(json.dumps(dict(exit=result,utc=datetime.datetime.now(datetime.timezone.utc).isoformat())))
print('SERVICE_EXIT',result)
