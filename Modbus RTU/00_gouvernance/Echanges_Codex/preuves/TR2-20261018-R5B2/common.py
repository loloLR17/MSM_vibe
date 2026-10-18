from pathlib import Path
import subprocess,json,time,datetime
P=Path('/home/lolo/dev/msm/reports/TR2-r5b2');W=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-r5b');E=W.parent/'TR2-r5b2-evidence'
PS='/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe'
def win(p):return subprocess.check_output(['wslpath','-w',str(p)],text=True).strip()
def service_args(name,port,seconds):
 return [str(W/'TR2.Supervision.Service.exe'),'--live-direct',port,'--allow-diagnostic','--database',win(E/'live.db'),'--listen','http://127.0.0.1:5089','--trace',win(E/(name+'-serial.jsonl')),'--seconds',str(seconds),'--unit','1','--connection-control',win(E/'connection-control')]
def service(name,port,seconds,toggle=False):
 args=service_args(name,port,seconds);(P/(name+'-service-command.json')).write_text(json.dumps(args,indent=2)+'\n')
 with (P/(name+'-service.log')).open('wb') as f:
  proc=subprocess.Popen(args,stdout=f,stderr=subprocess.STDOUT)
  try:
   with (P/(name+'-http.jsonl')).open('wb') as log:
    collector=subprocess.run([str(P/'HttpProbe02.exe'),'collect',str(max(seconds-1,1)),win(E/'connection-control'),'toggle' if toggle else 'steady'],stdout=log,stderr=subprocess.STDOUT,timeout=seconds+15)
   proc.wait(timeout=10);collector.check_returncode();assert proc.returncode==0,proc.returncode
  finally:
   if proc.poll() is None:proc.terminate();proc.wait(timeout=5)
 print(name,'service closed',flush=True)
