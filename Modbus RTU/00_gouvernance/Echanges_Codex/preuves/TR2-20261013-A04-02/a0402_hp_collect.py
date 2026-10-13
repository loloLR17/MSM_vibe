from pathlib import Path
import subprocess,base64,json,hashlib
p=Path(Path('/tmp/tr2-a0402-proof').read_text());body=r'''$ErrorActionPreference='Stop';$ProgressPreference='SilentlyContinue';[Console]::OutputEncoding=New-Object System.Text.UTF8Encoding($false)
if($env:COMPUTERNAME -ne 'LAPTOP-3F6J7STJ'){throw 'Unexpected HP'}
$out=@()
foreach($mode in @('Baseline','Capture')){
 $file=Join-Path $env:LOCALAPPDATA ('TR2\A0402-20261013T152741Z\'+$mode+'\result.json')
 $bytes=[IO.File]::ReadAllBytes($file)
 $out+=[ordered]@{mode=$mode;bytes=$bytes.Length;sha256=(Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash;base64=[Convert]::ToBase64String($bytes)}
}
ConvertTo-Json -InputObject $out -Depth 4
'''
bootstrap='[Console]::InputEncoding=New-Object System.Text.UTF8Encoding($false); & ([ScriptBlock]::Create([Console]::In.ReadToEnd()))';cmd=['/mnt/c/Windows/System32/OpenSSH/ssh.exe','-T','-o','BatchMode=yes','-o','StrictHostKeyChecking=yes','-o','ConnectTimeout=10','tr2-hp','powershell.exe -NoProfile -NonInteractive -EncodedCommand '+base64.b64encode(bootstrap.encode('utf-16le')).decode()]
z=subprocess.run(cmd,input=body.encode(),capture_output=True,timeout=30);(p/'hp-evidence-readback-stdout.log').write_bytes(z.stdout);(p/'hp-evidence-readback-stderr.log').write_bytes(z.stderr);z.check_returncode();items=json.loads(z.stdout.decode('utf-8-sig'))
for x in items:
 data=base64.b64decode(x.pop('base64'));assert len(data)==x['bytes'] and hashlib.sha256(data).hexdigest()==x['sha256'].lower();mode=x['mode'].lower();assert json.loads(data)==json.loads((p/('hp-'+mode+'-result.json')).read_text());(p/('hp-full-'+mode+'-result.json')).write_bytes(data)
(p/'hp-evidence-readback.json').write_text(json.dumps(dict(command=cmd,return_code=z.returncode,full_contents_read=True,no_serial_access=True,files=items),indent=2)+'\n');(p/'hp-evidence-readback.ps1').write_text(body);print('Both full HP journals read, contents and hashes verified; no serial access.')
