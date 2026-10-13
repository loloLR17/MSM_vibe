from pathlib import Path
import subprocess,base64,json,datetime
P=Path(Path('/tmp/tr2-a0402-proof').read_text());SCRIPT=P/'a04-02-read-only.ps1';SSH='/mnt/c/Windows/System32/OpenSSH/ssh.exe'
IMAGE='b4301d6f3325c266edc19297413fd0885828e5625ec030a8acad340444b3fd9b'
RUN='A0402-20261013T152741Z'
def invoke(mode,gate=None):
 name='hp-'+mode.lower();assert not (P/(name+'-result.json')).exists()
 body='& {\n'+SCRIPT.read_text()+'\n} -Mode '+repr(mode)+' -RunId '+repr(RUN)+' -ExpectedImageHash '+repr(IMAGE)
 if gate:body+=' -GateUtc '+repr(gate)
 bootstrap='[Console]::InputEncoding=New-Object System.Text.UTF8Encoding($false); & ([ScriptBlock]::Create([Console]::In.ReadToEnd()))'
 encoded=base64.b64encode(bootstrap.encode('utf-16le')).decode()
 cmd=[SSH,'-T','-o','BatchMode=yes','-o','StrictHostKeyChecking=yes','-o','ConnectTimeout=10','-o','ServerAliveInterval=5','-o','ServerAliveCountMax=2','tr2-hp','powershell.exe -NoProfile -NonInteractive -EncodedCommand '+encoded]
 before=datetime.datetime.now(datetime.timezone.utc);z=subprocess.run(cmd,input=body.encode('utf8'),capture_output=True,timeout=40);after=datetime.datetime.now(datetime.timezone.utc)
 (P/(name+'-stdout.log')).write_bytes(z.stdout);(P/(name+'-stderr.log')).write_bytes(z.stderr);(P/(name+'-request.ps1')).write_text(body)
 (P/(name+'-invocation.json')).write_text(json.dumps(dict(mode=mode,command=cmd,return_code=z.returncode,host_before=before.isoformat(),host_after=after.isoformat(),no_retry=True),indent=2)+'\n')
 z.check_returncode();result=json.loads(z.stdout.decode('utf-8-sig'));(P/(name+'-result.json')).write_text(json.dumps(result,indent=2)+'\n')
 if mode=='Preflight':
  offset=(datetime.datetime.fromisoformat(result['utc'])-after).total_seconds();(P/'hp-clock-offset.json').write_text(json.dumps(dict(offset_lower_bound_seconds=offset,remote=result['utc'],host_returned=after.isoformat()),indent=2)+'\n')
 print(mode,result,flush=True);return result
def gate():
 offset=json.loads((P/'hp-clock-offset.json').read_text())['offset_lower_bound_seconds']
 return (datetime.datetime.now(datetime.timezone.utc)+datetime.timedelta(seconds=offset)).isoformat()
if __name__=='__main__':invoke('Preflight')
