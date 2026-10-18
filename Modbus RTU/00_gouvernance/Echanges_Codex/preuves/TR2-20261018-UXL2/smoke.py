from pathlib import Path
import subprocess,urllib.request,urllib.parse,urllib.error,time,json,zlib,hashlib,signal
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-UXL2-trends-navigation');p=Path('/home/lolo/dev/msm/reports/TR2-uxl2');db=p/'historical-r5b2.db';raw=zlib.decompress((r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261018-R5B2/live.db.zlib').read_bytes());assert not db.exists();db.write_bytes(raw);before=hashlib.sha256(raw).hexdigest()
exe=r/'Modbus RTU/Supervision TR2/src/TR2.Supervision.Service/bin/Debug/net10.0/TR2.Supervision.Service.dll';cmd=['dotnet',str(exe),'--live-archive',str(db),'--listen','http://127.0.0.1:5094'];(p/'smoke-command.json').write_text(json.dumps(cmd,indent=2)+'\n')
with (p/'http-server.log').open('wb') as log:
 proc=subprocess.Popen(cmd,stdout=log,stderr=subprocess.STDOUT)
 try:
  for i in range(80):
   try:urllib.request.urlopen('http://127.0.0.1:5094/api/v2/live/overview',timeout=1).close();break
   except Exception:time.sleep(.1)
  params=dict(bus='live-direct',unit=1,**{'from':'2026-10-18T16:00:00Z','to':'2026-10-18T18:00:00Z'},limit=500)
  url='http://127.0.0.1:5094/api/v2/live/history?'+urllib.parse.urlencode(params)
  data=urllib.request.urlopen(url,timeout=5).read();(p/'history-http.json').write_bytes(data);page=json.loads(data);assert len(page['entries'])==500 and page['historical'] and not page['isCurrent'];assert page['nextBefore']
  for path,name in [('/api/v1/fleet','v1.json'),('/api/v2/live/overview','overview.json')]: (p/name).write_bytes(urllib.request.urlopen('http://127.0.0.1:5094'+path,timeout=5).read())
  out=Path('/mnt/c/Users/lolo/AppData/Local/MSM/UXL2-evidence-'+str(time.time_ns()));out.mkdir(parents=True)
  wpath=lambda f:subprocess.check_output(['wslpath','-w',str(f)],text=True).strip()
  with (p/'browser-command.log').open('wb') as browserlog:
   c=subprocess.run(['/mnt/c/Program Files/nodejs/node.exe',wpath(p/'browser.cjs'),wpath(out)],stdout=browserlog,stderr=subprocess.STDOUT,timeout=110)
  for file in out.iterdir(): (p/file.name).write_bytes(file.read_bytes())
  assert c.returncode==0,(p/'browser-command.log').read_text()
  assert hashlib.sha256(db.read_bytes()).hexdigest()==before and not Path(str(db)+'-wal').exists()
  (p/'smoke-summary.json').write_text(json.dumps(dict(pass_result=True,archive_sha256=before,archive_unchanged=True,no_WAL=True,physical=False,serial=False,points_observations=len(page['entries']),browsers=json.loads((p/'browser-summary.json').read_text())),indent=2)+'\n')
 finally:
  proc.send_signal(signal.SIGINT);proc.wait(timeout=10)
