"""Fixed, non-interactive Windows SSH tasks. Never opens a serial port."""
import argparse
import base64
import datetime
import json
from pathlib import Path
import subprocess

SSH = '/mnt/c/Windows/System32/OpenSSH/ssh.exe'
SCRIPT = Path(__file__).with_name('hp-task.ps1')
TASKS = ('Probe', 'Inventory', 'Offline', 'FailureProbe')


def ps_quote(value):
    return "'" + value.replace("'", "''") + "'"


def command(task, exe=None, sha=None):
    if task not in TASKS:
        raise ValueError('Unsupported task')
    if task == 'Offline' and (not exe or not sha):
        raise ValueError('Offline requires executable path and trusted SHA256')
    invocation = '& {\n' + SCRIPT.read_text(encoding='utf-8') + '\n} -Task ' + ps_quote(task)
    if exe is not None:
        invocation += ' -ExePath ' + ps_quote(exe)
    if sha is not None:
        invocation += ' -ExpectedSha256 ' + ps_quote(sha)
    bootstrap = "[Console]::InputEncoding = New-Object System.Text.UTF8Encoding($false); & ([ScriptBlock]::Create([Console]::In.ReadToEnd()))"
    encoded = base64.b64encode(bootstrap.encode('utf-16le')).decode('ascii')
    return [SSH, '-T', '-o', 'BatchMode=yes', '-o', 'StrictHostKeyChecking=yes',
            '-o', 'ConnectTimeout=10', '-o', 'ServerAliveInterval=5',
            '-o', 'ServerAliveCountMax=2', 'tr2-hp',
            'powershell.exe -NoProfile -NonInteractive -EncodedCommand ' + encoded], invocation.encode('utf-8')


def run(task, evidence, exe=None, sha=None, timeout=45):
    cmd, payload = command(task, exe, sha)
    evidence = Path(evidence)
    evidence.mkdir(parents=True, exist_ok=False)  # Never overwrite previous evidence.
    started = datetime.datetime.now(datetime.timezone.utc).isoformat()
    try:
        result = subprocess.run(cmd, input=payload, capture_output=True, timeout=timeout)
        code, stdout, stderr, expired = result.returncode, result.stdout, result.stderr, False
    except subprocess.TimeoutExpired as error:
        code, stdout, stderr, expired = 124, error.stdout or b'', error.stderr or b'', True
    except OSError as error:
        code, stdout, stderr, expired = 127, b'', str(error).encode(), False
    (evidence / 'stdout.log').write_bytes(stdout)
    (evidence / 'stderr.log').write_bytes(stderr)
    (evidence / 'result.json').write_text(json.dumps(dict(task=task, started=started,
        exit_code=code, timeout=expired, serial_authorized=False,
        timeout_limit_seconds=timeout, remote_completion_known=not expired and code != 255,
        note='SSH disconnect is not proof of remote cancellation; never retry automatically'), indent=2)+'\n')
    return code


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('task', choices=TASKS)
    parser.add_argument('--evidence', required=True)
    parser.add_argument('--exe')
    parser.add_argument('--sha256')
    args = parser.parse_args()
    try:
        return run(args.task, args.evidence, args.exe, args.sha256)
    except (ValueError, FileExistsError) as error:
        parser.error(str(error))


if __name__ == '__main__':
    raise SystemExit(main())
