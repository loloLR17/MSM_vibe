from pathlib import Path
import subprocess,urllib.request,urllib.error,time,json,zlib,hashlib
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-UXL1-live-readonly');p=Path('/home/lolo/dev/msm/reports/TR2-uxl1');db=p/'historical-r5b2.db';assert not db.exists();db.write_bytes(zlib.decompress((r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261018-R5B2/live.db.zlib').read_bytes()));before=hashlib.sha256(db.read_bytes()).hexdigest()
exe=r/'Modbus RTU/Supervision TR2/src/TR2.Supervision.Service/bin/Debug/net10.0/TR2.Supervision.Service.dll';cmd=['dotnet',str(exe),'--live-archive',str(db),'--listen','http://127.0.0.1:5093'];(p/'smoke-command.json').write_text(json.dumps(cmd,indent=2)+'\n')
with (p/'http-server.log').open('wb') as log:
 proc=subprocess.Popen(cmd,stdout=log,stderr=subprocess.STDOUT)
 try:
  for i in range(60):
   try:urllib.request.urlopen('http://127.0.0.1:5093/api/v2/live/overview',timeout=1).close();break
   except Exception:time.sleep(.1)
  rows=[]
  for path,name in [('/','index.html'),('/live-fleet.html','fleet.html'),('/live-device.html?deviceId=1','detail.html'),('/live.html','diagnostic.html'),('/api/v2/live/overview','overview.json'),('/api/v1/fleet','v1.json')]:
   with urllib.request.urlopen('http://127.0.0.1:5093'+path,timeout=2) as z:
    b=z.read();assert z.status==200;(p/('http-'+name)).write_bytes(b);rows.append(dict(path=path,status=z.status,bytes=len(b)))
  data=json.loads((p/'http-overview.json').read_text());device=next(x for x in data['devices'] if x['observation']['deviceId']);assert device['historical'] and not device['isCurrent'];assert device['observation']['snapshot']['held']
  edge='/mnt/c/Program Files (x86)/Microsoft/Edge/Application/msedge.exe';wpath=lambda f:subprocess.check_output(['wslpath','-w',str(f)],text=True).strip()
  browser=[]
  for name,path in [('fleet','/live-fleet.html'),('detail','/live-device.html?deviceId=1&view=vibration')]:
   args=[edge,'--headless=new','--disable-gpu','--no-first-run','--no-default-browser-check','--user-data-dir='+wpath(Path('/mnt/c/Users/lolo/AppData/Local/MSM/UXL1-headless-'+name)),'--dump-dom','--virtual-time-budget=1500','http://127.0.0.1:5093'+path]
   with (p/(name+'-dom.html')).open('wb') as out,(p/(name+'-browser.log')).open('wb') as err:z=subprocess.run(args,stdout=out,stderr=err,timeout=30)
   text=(p/(name+'-dom.html')).read_text(errors='replace');ok=all(x in text for x in ['Historique','LiveDiagnostic','NON-PRODUCTION','mg']);assert z.returncode==0 and ok,(name,text[:1200]);browser.append(dict(page=path,return_code=z.returncode,real_headless_DOM=True,human_observation=False))
  request=urllib.request.Request('http://127.0.0.1:5093/api/v1/devices/1/commands',data=b'{}',headers={'Content-Type':'application/json'})
  try:urllib.request.urlopen(request,timeout=2);raise AssertionError('Command unexpectedly available')
  except urllib.error.HTTPError as e:assert e.code==404
  assert hashlib.sha256(db.read_bytes()).hexdigest()==before and not Path(str(db)+'-wal').exists()
  (p/'smoke-summary.json').write_text(json.dumps(dict(http=rows,browsers=browser,archive_sha256=before,archive_byte_identical=True,no_WAL=True,physical=False,serial_factory=False,command_post_status=404,history='Real R5B2 stored observations, HISTORICAL ONLY',overview_devices=len(data['devices'])),indent=2)+'\n')
 finally:
  proc.send_signal(__import__('signal').SIGINT)
  proc.wait(timeout=10)
