from pathlib import Path
import subprocess,threading,queue,re,time,json,urllib.request,zlib,signal,datetime,sqlite3
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R5A-pc-live');p=Path('/home/lolo/dev/msm/reports/TR2-r5a');capture=p/'paced-master.jsonl';capture.write_bytes(zlib.decompress((r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261017-R4V/hardware/paced-master.jsonl.zlib').read_bytes()))
cmd=['dotnet',str(r/'Modbus RTU/Supervision TR2/src/TR2.Supervision.Service/bin/Debug/net10.0/TR2.Supervision.Service.dll'),'--live-replay',str(capture),'--database',str(p/'smoke02.db'),'--listen','http://127.0.0.1:0'];assert not (p/'smoke02.db').exists()
q=queue.Queue();proc=subprocess.Popen(cmd,cwd=r/'Modbus RTU/Supervision TR2/src/TR2.Supervision.Service',stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
def pump():
 with (p/'smoke02-server.log').open('w') as f:
  for line in proc.stdout:f.write(line);f.flush();q.put(line)
threading.Thread(target=pump,daemon=True).start();responses={}
try:
 end=time.monotonic()+20;base=None
 while time.monotonic()<end:
  line=q.get(timeout=20)
  m=re.search(r'Now listening on: (http://127\.0\.0\.1:\d+)',line)
  if m:base=m.group(1);break
 assert base
 for endpoint in ['/','/live.html','/live.js','/api/v1/fleet']:
  with urllib.request.urlopen(base+endpoint,timeout=3) as response:
   data=response.read();responses[endpoint]=dict(status=response.status,bytes=len(data));(p/('smoke02-'+endpoint.strip('/').replace('/','-')+'.txt')).write_bytes(data)
 end=time.monotonic()+15
 while time.monotonic()<end:
  with urllib.request.urlopen(base+'/api/v2/live',timeout=3) as response:body=response.read();views=json.loads(body)
  if views and views[0]['observation']['continuity']=='Rejected':break
  time.sleep(.2)
 assert views and views[0]['observation']['continuity']=='Rejected' and not views[0]['isCurrent']
 o=views[0]['observation'];assert o['rejectedB3Value']['severityGlobal']==240 and o['snapshot'] is None and o['runId'] is None and o['epoch'] is None
 assert 'simulated' in o['source'] and o['identity']['profile']=='LiveDiagnostic'
 (p/'smoke02-api-live.json').write_bytes(body)
 responses['/api/v2/live']=dict(status=200,isCurrent=False,continuity='Rejected',severityRaw=240,source=o['source'])
finally:
 proc.send_signal(signal.SIGINT)
 try:proc.wait(timeout=8)
 except subprocess.TimeoutExpired:proc.terminate();proc.wait(timeout=5)
assert proc.returncode==0,proc.returncode
with sqlite3.connect(p/'smoke02.db') as db:
 count,bad=db.execute("select count(*),sum(case when continuity='Rejected' and values_valid=0 and fresh=0 then 1 else 0 end) from live_observation").fetchone();assert bad>0
result=dict(result='PASS_OFFLINE',command=cmd,utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),responses=responses,sqliteRows=count,quarantinedRows=bad,noSerialAccess=True,noHardwareAccess=True,returnCode=proc.returncode);(p/'smoke02-summary.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
