#!/usr/bin/python3
"""Git/GitHub CLI credential bridge to the Windows user credential vault.

Secrets travel only through pipes and the child environment. Never log them.
"""
import os
from pathlib import Path
import subprocess
import sys

CMD = '/mnt/c/Windows/System32/cmd.exe'
GCM = r'C:\Users\lolo\AppData\Local\T1000\gcm.cmd'
QUERY = 'protocol=https\nhost=github.com\nusername=loloLR17\n\n'

def gcm(action, payload):
    result = subprocess.run([CMD, '/d', '/c', GCM, action], input=payload,
                            text=True, stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE, cwd='/mnt/c/Users/lolo')
    if result.returncode:
        raise RuntimeError('Windows credential vault unavailable; no plaintext fallback')
    return result.stdout

def credential():
    data = dict(line.split('=', 1) for line in gcm('get', QUERY).splitlines() if '=' in line)
    if not data.get('password'):
        raise RuntimeError('No GitHub credential in the Windows user vault')
    return data

def main():
    mode = sys.argv[1]
    if mode == 'git':
        action = sys.argv[2]
        fields = dict(line.split('=', 1) for line in sys.stdin.read().splitlines() if '=' in line)
        if fields.get('protocol') != 'https' or fields.get('host') != 'github.com':
            return 0
        if action == 'get':
            data = credential()
            print('username=' + data['username'])
            print('password=' + data['password'])
            print()
        # Git store/erase do not silently replace or revoke the shared gh token.
        return 0
    if mode == 'gh':
        if sys.argv[2:4] in (['auth', 'login'], ['auth', 'logout'], ['auth', 'refresh']):
            raise RuntimeError('Use /usr/bin/gh for interactive login, then gcm-bridge.py import-gh')
        env = os.environ.copy()
        env['GH_TOKEN'] = credential()['password']
        os.execve('/usr/bin/gh', ['/usr/bin/gh', *sys.argv[2:]], env)
    if mode == 'import-gh':
        # Keep the original login until both vault retrieval and API identity pass.
        token = subprocess.check_output(['/usr/bin/gh', 'auth', 'token'], text=True).strip()
        if not token or '\n' in token:
            raise RuntimeError('Invalid token response')
        gcm('store', QUERY.rstrip('\n') + '\npassword=' + token + '\n\n')
        recovered = credential()['password']
        if recovered != token:
            raise RuntimeError('Credential vault round-trip mismatch')
        env = os.environ.copy()
        env['GH_TOKEN'] = recovered
        who = subprocess.check_output(['/usr/bin/gh', 'api', 'user', '--jq', '.login'], env=env, text=True).strip()
        if who != 'loloLR17':
            raise RuntimeError('Unexpected GitHub account')
        hosts = Path.home()/'.config/gh/hosts.yml'
        content = hosts.read_text()
        lines = []
        skip = False
        for line in content.splitlines():
            if line and not line[0].isspace():
                skip = line == 'github.com:'
                if skip:
                    lines.extend(['github.com:', '    git_protocol: https'])
                    continue
            if not skip:
                lines.append(line)
        cleaned = '\n'.join(lines) + '\n'
        # Atomic replacement with no plaintext backup.
        temp = hosts.with_name('hosts.yml.secure-migration')
        temp.write_text(cleaned)
        temp.chmod(0o600)
        temp.replace(hosts)
        print('Windows credential vault verified; GitHub account loloLR17; plaintext oauth_token removed')
        return 0
    raise RuntimeError('Unknown bridge operation')

if __name__ == '__main__':
    try:
        sys.exit(main())
    except Exception as error:
        # Only fixed diagnostic messages; never emit subprocess buffers/tokens.
        print(str(error) if isinstance(error, RuntimeError) else 'Credential bridge operation failed', file=sys.stderr)
        sys.exit(1)
