from pathlib import Path
import subprocess,json,datetime,sys
p=Path(__file__).parent;a=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-LIVE-COM4-HELD-20261019');d=a/'data';action=sys.argv[1];assert action in ['start','stop'];e=p/(action+'.jsonl');assert not e.exists()
win=lambda x:subprocess.check_output(['wslpath','-w',str(x)],text=True).strip()
cmd=[str(a/'app/TR2.Supervision.Service.exe'),'--live-command',action,'--port','COM4','--control',win(d/'connection-control'),'--state',win(d/'transactions.json')]
(p/(action+'-invocation.json')).write_text(json.dumps(dict(command=cmd,utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),ledger_before=json.loads((d/'transactions.json').read_text())),indent=2))
with e.open('wb') as f:r=subprocess.run(cmd,stdout=f,stderr=subprocess.STDOUT,timeout=230)
(p/(action+'-exit.json')).write_text(json.dumps(dict(returncode=r.returncode,utc=datetime.datetime.now(datetime.timezone.utc).isoformat())));print(action,r.returncode);assert r.returncode==0
