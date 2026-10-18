from common import *
exe='/mnt/c/Program Files (x86)/Microsoft/Edge/Application/msedge.exe'
# Independent signed browser, fresh diagnostic-only profile; does not own any serial handle.
log=P/'running-service.log';deadline=time.monotonic()+200
while not(log.exists() and b'"IsCurrent":true' in log.read_bytes()):
 assert time.monotonic()<deadline,'No current physical service view for browser';time.sleep(.2)
cmd=[exe,'--headless=new','--disable-gpu','--no-first-run','--no-default-browser-check','--user-data-dir='+win(E/'edge-diagnostic-profile'),'--dump-dom','--virtual-time-budget=2000','http://127.0.0.1:5089/live.html']
(P/'browser-command.json').write_text(json.dumps(cmd,indent=2)+'\n')
with (P/'browser-running-dom.html').open('wb') as out, (P/'browser-running-stderr.log').open('wb') as err:
 z=subprocess.run(cmd,stdout=out,stderr=err,timeout=30)
(P/'browser-result.json').write_text(json.dumps(dict(return_code=z.returncode,utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),mode='headless Edge fresh diagnostic profile; not human browser observation',bytes=(P/'browser-running-dom.html').stat().st_size),indent=2)+'\n')
print('Browser return',z.returncode)
