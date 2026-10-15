#!/usr/bin/env python3
"""Count physical lines from Git blobs; no generated/vendor/build or data lines.
WORKTREE includes tracked + nonignored untracked files. Run from repository root.
No code/comment/blank distinction: splitlines() counts physical lines exactly.
"""
import argparse
import io
import json
from pathlib import Path
import subprocess
import tarfile

SOURCE = {'.c', '.h', '.cc', '.cpp', '.cxx', '.hpp', '.hh'}
TEST_SOURCE = SOURCE | {'.py', '.cs', '.sh'}
EXCLUDED = {'vendor', 'generated', 'drivers', 'middlewares', 'third_party', 'thirdparty', 'node_modules'}

def category(name):
    p = Path(name)
    parts = {x.lower() for x in p.parts}
    if parts & EXCLUDED or any(x.lower().startswith('build') for x in p.parts):
        return None
    if p.suffix.lower() in TEST_SOURCE and ('tests' in parts or 'test' in parts or p.name.startswith('test_')):
        return 'tests'
    if p.suffix.lower() in {'.md', '.rst', '.adoc'}:
        return 'documentation'
    if p.suffix.lower() in SOURCE and (name.startswith('Modbus RTU/05_Firmware/') or name.startswith('experiments/')):
        return 'owned_firmware_c_cpp'
    return None

def contents(ref):
    if ref == 'WORKTREE':
        names = subprocess.check_output(['git', 'ls-files', '-z', '--cached', '--others', '--exclude-standard']).split(b'\0')
        for name in sorted(set(x.decode() for x in names if x)):
            if category(name) and Path(name).is_file():
                yield name, Path(name).read_bytes()
    else:
        # Archive streams the exact revision, never reads a stale local checkout.
        process = subprocess.Popen(['git', 'archive', ref], stdout=subprocess.PIPE)
        with tarfile.open(fileobj=process.stdout, mode='r|') as archive:
            for member in archive:
                if member.isfile() and category(member.name):
                    yield member.name, archive.extractfile(member).read()
        if process.wait():
            raise RuntimeError('git archive failed: ' + ref)

def measure(ref):
    counts = {x: {'files': 0, 'physical_lines': 0} for x in ('owned_firmware_c_cpp', 'tests', 'documentation')}
    for name, data in contents(ref):
        count = counts[category(name)]
        count['files'] += 1
        count['physical_lines'] += len(data.splitlines())
    return counts

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--refs', nargs='+', default=['b6bae880', '3f3ca1c', 'WORKTREE'])
    args = parser.parse_args()
    result = {'method': 'physical splitlines; firmware includes owned experiments/historical candidates, NOT only ELF-linked sources; tests include host/PC source; documentation md/rst/adoc; vendor/generated/build excluded; JSON/CSV/log/proofs data not LOC',
              'measurements': {ref: measure(ref) for ref in args.refs}}
    base = result['measurements'][args.refs[0]]
    result['net_lines_from_first'] = {ref: {key: counts[key]['physical_lines'] - base[key]['physical_lines'] for key in counts}
                                      for ref, counts in result['measurements'].items()}
    print(json.dumps(result, indent=2))

if __name__ == '__main__':
    main()
