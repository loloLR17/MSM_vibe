from pathlib import Path
import subprocess,json,datetime,sys
p=Path('/home/lolo/dev/msm/reports/TR2-windows-trust-live-resume/physical'); a=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-LIVE-DEMO-20261019'); d=a/'data'
win=lambda x:subprocess.check_output(['wslpath','-w',str(x)],text=True).strip()
action=sys.argv[1]; name=sys.argv[2]; assert action in ('start','stop')
cmd=[str(a/'app/TR2.Supervision.Service.exe'),'--live-command',action,'--port','COM4','--control',win(d/'connection-control'),'--state',win(d/'transactions.json')]
assert not (p/(name+'.jsonl')).exists()
(p/(name+'-command.json')).write_text(json.dumps(dict(command=cmd,utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),tokenized_ack_and_OS_exclusive_required=True,c36c971_known_ack_oscillation_accepted=True),indent=2))
with (p/(name+'.jsonl')).open('wb') as log: z=subprocess.run(cmd,stdout=log,stderr=subprocess.STDOUT,timeout=230)
(p/(name+'-exit.json')).write_text(json.dumps(dict(exit=z.returncode,utc=datetime.datetime.now(datetime.timezone.utc).isoformat())))
print(name,'exit',z.returncode)
