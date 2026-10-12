"""Explicit bounded M014-H SSH tasks; Physical requires a fresh MCU gate timestamp."""
import argparse
import base64
import json
import subprocess
from pathlib import Path
SSH='/mnt/c/Windows/System32/OpenSSH/ssh.exe'
def command(task,metadata,gate=None):
    if task not in ('Prepare','Offline','Physical','Steady'): raise ValueError('Unsupported task')
    if task in ('Physical','Steady') and not gate: raise ValueError('Fresh target gate required')
    quote=lambda s: "'"+s.replace("'", "''")+"'"
    body='& {\n'+Path(__file__).with_name('commissioning-h.ps1').read_text()+'\n} -Task '+quote(task)
    for param,key in [('PackageName','package_name'),('ExpectedZipHash','zip_sha256'),('ExpectedManifestHash','manifest_sha256'),('ExpectedB0Hash','b0_sha256')]:body+=' -'+param+' '+quote(metadata[key])
    if gate:body+=' -GateUtc '+quote(gate)
    if metadata.get('run_name') in ('physical-m015-a-restored','physical-m015-b-restored','physical-m015-c-restored','physical-m015-d-restored','physical-m015-f-restored') and task != 'Steady':
        raise ValueError('M015 restoration permits only two read-only requests')
    if 'run_name' in metadata:
        if metadata['run_name'] not in ('physical-once','physical-after-b1-fix','physical-steady-final','physical-m015-a-restored','physical-m015-b-restored','physical-m015-c-restored','physical-m015-d-restored','physical-m015-f-restored'):raise ValueError('Unreviewed run name')
        body+=' -RunName '+quote(metadata['run_name'])
    bootstrap="[Console]::InputEncoding=New-Object System.Text.UTF8Encoding($false); & ([ScriptBlock]::Create([Console]::In.ReadToEnd()))"
    encoded=base64.b64encode(bootstrap.encode('utf-16le')).decode()
    return [SSH,'-T','-o','BatchMode=yes','-o','StrictHostKeyChecking=yes','-o','ConnectTimeout=10','-o','ServerAliveInterval=5','-o','ServerAliveCountMax=2','tr2-hp','powershell.exe -NoProfile -NonInteractive -EncodedCommand '+encoded],body.encode()
def main():
    a=argparse.ArgumentParser();a.add_argument('task',choices=['Prepare','Offline','Physical','Steady']);a.add_argument('--metadata',required=True);a.add_argument('--evidence',required=True);a.add_argument('--gate-utc');args=a.parse_args()
    cmd,body=command(args.task,json.loads(Path(args.metadata).read_text()),args.gate_utc)
    p=Path(args.evidence);p.mkdir(exist_ok=False)
    (p/'request.ps1').write_bytes(body)
    try:
        result=subprocess.run(cmd,input=body,capture_output=True,timeout=320 if args.task=='Physical' else 90)
        (p/'stdout.log').write_bytes(result.stdout);(p/'stderr.log').write_bytes(result.stderr)
        (p/'result.json').write_text(json.dumps({'exit_code':result.returncode,'physical_authorized':args.task in ('Physical','Steady'),'no_retry':True,'remote_completion_known':result.returncode!=255})+'\n')
        print(result.stdout.decode('utf-8',errors='replace'));print(result.stderr.decode('utf-8',errors='replace'))
        return result.returncode
    except subprocess.TimeoutExpired as error:
        (p/'stdout.log').write_bytes(error.stdout or b'');(p/'stderr.log').write_bytes(error.stderr or b'')
        (p/'result.json').write_text('{"exit_code":124,"remote_completion_known":false,"no_retry":true}\n');return 124
if __name__=='__main__':raise SystemExit(main())
