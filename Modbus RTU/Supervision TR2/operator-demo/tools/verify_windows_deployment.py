"""WSL developer gate: actual Windows execution under its unchanged policy, never COM4.
A successful gate is specific to these bytes and this policy/time, not an auto-allow.
"""
from pathlib import Path
import argparse, datetime, hashlib, json, subprocess, sys

PS = '/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe'

def win(path):
    return subprocess.check_output(['wslpath', '-w', str(path)], text=True).strip()

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--application', type=Path, required=True)
    parser.add_argument('--evidence', type=Path, required=True)
    parser.add_argument('--port', type=int, default=5102)
    parser.add_argument('--startup-timeout', type=int, default=60, help='HTTP readiness deadline in seconds; cold Windows startup can exceed 25s')
    args = parser.parse_args()
    if not 1 <= args.startup_timeout <= 120:
        parser.error('startup-timeout must be 1..120 seconds')
    if not 1024 <= args.port <= 65535:
        parser.error('port must be 1024..65535')
    app = args.application.resolve()
    exe = app / 'TR2.Supervision.Service.exe'
    if not exe.is_file():
        parser.error('published Windows executable missing')
    evidence = args.evidence.resolve()
    evidence.mkdir(parents=True, exist_ok=False)  # Never overwrite an earlier gate.
    fingerprint = {f.relative_to(app).as_posix(): hashlib.sha256(f.read_bytes()).hexdigest()
                   for f in app.rglob('*') if f.is_file()}
    (evidence / 'application-sha256.json').write_text(json.dumps(fingerprint, indent=2)+'\n')
    # Data is on Windows local NTFS; isolated from the real physical database.
    data = Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-Windows-Gates') / evidence.name
    data.mkdir(parents=True, exist_ok=False)
    cmd = [str(exe), '--live-direct', 'COM99', '--allow-diagnostic', '--database', win(data/'smoke.db'),
           '--listen', f'http://127.0.0.1:{args.port}', '--trace', win(data/'serial.jsonl'),
           '--seconds', '20', '--unit', '1', '--connection-control', win(data/'control')]
    start = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (evidence/'command.json').write_text(json.dumps(dict(command=cmd, start_utc=start), indent=2)+'\n')
    http_script = r"""
[Console]::OutputEncoding=New-Object System.Text.UTF8Encoding($false)
$ErrorActionPreference='Stop';$base='http://127.0.0.1:PORT';$until=(Get-Date).AddSeconds(STARTUP_SECONDS);$ready=$false
while(-not $ready -and (Get-Date) -lt $until) {
 try {$first=Invoke-WebRequest -UseBasicParsing ($base+'/api/v2/live/overview') -TimeoutSec 1;$ready=$first.StatusCode -eq 200} catch {Start-Sleep -Milliseconds 150}
}
if(-not $ready){throw 'Windows host/overview did not become ready'}
$result=@();foreach($path in @('/api/v1/fleet','/api/v2/live/overview','/api/v2/live/history?bus=live-direct&unit=1&from=2026-10-19T00:00:00Z&to=2026-10-20T00:00:00Z','/live-device.html?deviceId=1&view=vibration')) {
 $r=Invoke-WebRequest -UseBasicParsing ($base+$path) -TimeoutSec 3
 $result += [pscustomobject]@{Path=$path;Status=[int]$r.StatusCode;Body=$r.Content}
};$result | ConvertTo-Json -Depth 6
""".replace('PORT', str(args.port)).replace('STARTUP_SECONDS', str(args.startup_timeout))
    with (evidence/'console.log').open('wb') as log:
        proc = subprocess.Popen(cmd, stdout=log, stderr=subprocess.STDOUT)
        try:
            with (evidence/'http.json').open('wb') as out:
                probe = subprocess.run([PS, '-NoProfile', '-Command', http_script], stdout=out,
                                       stderr=subprocess.STDOUT, timeout=args.startup_timeout+15)
            proc.wait(timeout=35)
        finally:
            if proc.poll() is None:
                proc.terminate(); proc.wait(timeout=10)
    query = r"""
[Console]::OutputEncoding=New-Object System.Text.UTF8Encoding($false)
$start=[DateTimeOffset]::Parse('START').UtcDateTime
$events=@(Get-WinEvent -FilterHashtable @{LogName='Microsoft-Windows-CodeIntegrity/Operational';Id=3033,3077;StartTime=$start} -ErrorAction SilentlyContinue |
 Where-Object {$_.Message -like '*APP_FILTER*'} |
 ForEach-Object {[pscustomobject]@{Id=$_.Id;RecordId=$_.RecordId;Utc=$_.TimeCreated.ToUniversalTime().ToString('o');Message=$_.Message;Xml=$_.ToXml()}})
[pscustomobject]@{BlockedCount=$events.Count;Events=$events;SmartAppControlState=(Get-ItemProperty 'HKLM:\SYSTEM\CurrentControlSet\Control\CI\Policy' -ErrorAction Stop).VerifiedAndReputablePolicyState} | ConvertTo-Json -Depth 6
""".replace('START', start).replace('APP_FILTER', win(app).split(':', 1)[-1].replace("'", "''"))
    with (evidence/'events.json').open('wb') as out:
        event_probe = subprocess.run([PS, '-NoProfile', '-Command', query], stdout=out,
                                     stderr=subprocess.STDOUT, timeout=20)
    result = dict(start_utc=start, end_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  service_exit=proc.returncode, http_probe_exit=probe.returncode,
                  event_probe_exit=event_probe.returncode, physical=False, port='COM99 nonexistent', startup_timeout_seconds=args.startup_timeout,
                  security_changes=False, application_files=len(fingerprint), future_builds_not_auto_allowed=True)
    result['files_unchanged'] = all(hashlib.sha256((app/name).read_bytes()).hexdigest()==digest
                                    for name,digest in fingerprint.items())
    result['pass'] = False
    try:
        events=json.loads((evidence/'events.json').read_bytes().decode('utf-8-sig'))
        result.update(correlated_blocks=events['BlockedCount'], policy_state=events['SmartAppControlState'])
    except (ValueError, KeyError):
        result['event_evidence_error'] = True
    try:
        rows=json.loads((evidence/'http.json').read_bytes().decode('utf-8-sig'))
        result['http'] = [dict(path=row['Path'],status=row['Status']) for row in rows]
        result['pass'] = proc.returncode==probe.returncode==event_probe.returncode==0 and result['files_unchanged'] and \
                         result.get('correlated_blocks')==0 and result.get('policy_state')==1 and len(rows)==4 and all(row['Status']==200 for row in rows)
    except (ValueError, KeyError):
        result['http_evidence_error'] = True
    (evidence/'summary.json').write_text(json.dumps(result, indent=2)+'\n')
    print(json.dumps(result, indent=2))
    return 0 if result['pass'] else 1

if __name__ == '__main__':
    sys.exit(main())
