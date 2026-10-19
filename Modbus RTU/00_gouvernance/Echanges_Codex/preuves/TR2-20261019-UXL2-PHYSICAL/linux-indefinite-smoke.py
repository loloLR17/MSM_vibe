from pathlib import Path
import subprocess,urllib.request,urllib.error,json,time,signal
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-UXL2-physical-live-ui');p=Path('/home/lolo/dev/msm/reports/TR2-uxl2-physical');d=p/'linux-smoke';d.mkdir();control=d/'control';exe=r/'Modbus RTU/Supervision TR2/src/TR2.Supervision.Service/bin/Debug/net10.0/TR2.Supervision.Service.dll'
cmd=['dotnet',str(exe),'--live-direct','COM99','--allow-diagnostic','--database',str(d/'live.db'),'--listen','http://127.0.0.1:5101','--trace',str(d/'serial.jsonl'),'--seconds','0','--unit','1','--connection-control',str(control)]
(p/'linux-smoke-command.json').write_text(json.dumps(cmd,indent=2)+'\n')
with (d/'console.log').open('wb') as log:
 proc=subprocess.Popen(cmd,stdout=log,stderr=subprocess.STDOUT)
 try:
  for i in range(60):
   try:urllib.request.urlopen('http://127.0.0.1:5101/api/v2/live/overview',timeout=1).close();break
   except Exception:time.sleep(.1)
  time.sleep(2);assert proc.poll() is None
  token='disconnected:linux-nonphysical-smoke';control.write_text(token)
  for i in range(50):
   if Path(str(control)+'.released').exists() and Path(str(control)+'.released').read_text()==token:break
   time.sleep(.1)
  assert Path(str(control)+'.released').read_text()==token
  overview=json.loads(urllib.request.urlopen('http://127.0.0.1:5101/api/v2/live/overview').read());(d/'paused-overview.json').write_text(json.dumps(overview,indent=2)+'\n')
  control.unlink()
  for i in range(30):
   if not Path(str(control)+'.released').exists():break
   time.sleep(.1)
  assert not Path(str(control)+'.released').exists()
  assert proc.poll() is None
 finally:
  proc.send_signal(signal.SIGINT);proc.wait(timeout=10)
 assert proc.returncode==0
(p/'linux-smoke-summary.json').write_text(json.dumps(dict(result='PASS',seconds_zero_remains_alive=True,token_pause_ack=True,resume_clears_ack=True,ctrl_c_clean_exit=True,physical=False,port='COM99 nonexistent',not_windows_qualification=True),indent=2)+'\n')
