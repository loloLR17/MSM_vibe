from pathlib import Path
import json,subprocess
p=Path('/home/lolo/dev/msm/reports/TR2-windows-trust-live-resume/physical')
n=__import__('sys').argv[1]
r=[json.loads(x) for x in (p/(n+'.jsonl')).read_text().splitlines() if 'terminal_and_effect_observed' in x]
assert len(r)==1 and r[0]['command']=='stop' and r[0]['b3'][0]==1 and r[0]['b3'][1]&512 and not r[0]['b3'][1]&4
f=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-LIVE-DEMO-20261019/data/connection-control.shutdown')
assert not f.exists();f.write_text('STOP terminal confirmed; shut down observation host only\n')
print('SHUTDOWN_REQUESTED_AFTER_STOP')
