#!/usr/bin/env python3
"""Offline-only builds/tests and immutable source verification; never opens a probe."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--cube', type=Path, required=True)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    sources = json.loads((ROOT / 'vendor/SOURCES.json').read_text())
    for item in sources['files']:
        assert hashlib.sha256((ROOT / item['path']).read_bytes()).hexdigest() == item['sha256']
    results = []
    def run(name, command):
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (args.output / (name + '.log')).write_bytes(result.stdout)
        results.append({'name': name, 'command': list(map(str, command)), 'exit_code': result.returncode})
        (args.output / 'commands.json').write_text(json.dumps(results, indent=2) + '\n')
        if result.returncode:
            print(result.stdout.decode(errors='replace'))
            raise SystemExit(result.returncode)
        print(name, 'PASS')
    host = args.output / 'host'
    run('host-configure', ['cmake', '-S', str(ROOT), '-B', str(host), '-G', 'Ninja',
                           '-DBENCH_HOST_TESTS=ON', '-DCMAKE_BUILD_TYPE=Debug'])
    run('host-build', ['cmake', '--build', str(host)])
    run('host-test', ['ctest', '--test-dir', str(host), '-V'])
    artifacts = []
    for example, profile in [('who', 'ST_STRICT'), ('polling', 'ST_STRICT'), ('fifo', 'ST_STRICT'), ('fifo', 'STATUS10')]:
        name = example + '-' + profile
        build = args.output / name
        run(name + '-configure', ['cmake', '-S', str(ROOT), '-B', str(build), '-G', 'Ninja',
            '-DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake', '-DCMAKE_BUILD_TYPE=Debug',
            '-DSTM32CUBE_U5_ROOT=' + str(args.cube), '-DBENCH_EXAMPLE=' + example,
            '-DBENCH_PROFILE=' + profile])
        run(name + '-build', ['cmake', '--build', str(build)])
        for suffix in ['elf', 'bin', 'map']:
            file = build / ('iis3dwb_reference.' + suffix)
            artifacts.append({'path': str(file), 'sha256': hashlib.sha256(file.read_bytes()).hexdigest(),
                              'bytes': file.stat().st_size})
    (args.output / 'artifacts.json').write_text(json.dumps(artifacts, indent=2) + '\n')
    # Exact evaluated source snapshot, including ST input and local adaptations.
    snapshot = {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest()
                for p in ROOT.rglob('*') if p.is_file() and '__pycache__' not in str(p)}
    (args.output / 'source-hashes.json').write_text(json.dumps(snapshot, indent=2) + '\n')

if __name__ == '__main__':
    main()
