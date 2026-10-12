"""Replay published evidence without hardware access or third-party packages."""
import csv
import hashlib
import json
import math
from pathlib import Path
import statistics
import struct

root = Path(__file__).resolve().parent
manifest = json.loads((root / 'manifest.json').read_text())
for item in manifest['files']:
    data = (root / item['path']).read_bytes()
    assert len(data) == item['bytes'], item['path']
    assert hashlib.sha256(data).hexdigest() == item['sha256'], item['path']
keys = ['begin_cycles', 'end_cycles', 'phase', 'status', 'result',
        'raw_x', 'raw_y', 'raw_z', 'x_mg', 'y_mg', 'z_mg', 'valid', 'saturated']
for name in ['initial', 'single', 'slow', 'mode3']:
    analysis = json.loads((root / (name + '-analysis.json')).read_text())
    with (root / (name + '-samples.csv')).open() as stream:
        rows = [[int(row[k]) for k in keys] for row in csv.DictReader(stream)]
    binary = b''.join(struct.pack('<5I6i2I', *row) for row in rows)
    assert len(rows) == 2048
    assert hashlib.sha256(binary).hexdigest() == analysis['raw_bin_sha256']
    assert all(row[11] == 1 and row[3] & 1 and row[4] == 0 for row in rows)
    assert all(b[0] > a[1] for a, b in zip(rows, rows[1:]))
    for row in rows:
        for raw_index in (5, 6, 7):
            raw = row[raw_index]
            expected = (1 if raw >= 0 else -1) * ((abs(raw) * 122 + 500) // 1000)
            assert row[raw_index + 3] == expected
    for phase in analysis['phases']:
        data = [row for row in rows if row[2] == phase['phase']]
        assert len(data) == 512
        assert len(set(tuple(row[5:8]) for row in data)) == phase['distinct_raw_xyz']
        assert sum(row[12] for row in data) == phase['saturated']
        for i, raw_index in enumerate((5, 6, 7)):
            mean = statistics.mean(row[raw_index] * .122 for row in data)
            assert math.isclose(mean, phase['mean_mg_nominal'][i], abs_tol=1e-9)
        norm = statistics.mean(math.sqrt(sum((row[k] * .122) ** 2 for k in (5, 6, 7))) for row in data)
        assert math.isclose(norm, phase['norm_mean_mg'], abs_tol=1e-9)
        hz = (len(data) - 1) * analysis['metadata']['core_hz'] / (data[-1][1] - data[0][1])
        assert math.isclose(hz, phase['observed_read_rate_hz'], rel_tol=1e-12)
    if name == 'mode3':
        assert all(analysis['checks'].values())
    else:
        assert not analysis['checks']['zero_saturation']
        assert not analysis['checks']['baseline_norm_800_1200mg']
    print(name + ': 2048 samples reconstructed; raw hash, conversions, time and statistics match')
with (root / 'hp-restored-physical.jsonl').open() as stream:
    events = [json.loads(line) for line in stream]
assert [(e['start'], e['count']) for e in events if e['event'] == 'read_intent'] == [(0, 21), (1000, 20)]
assert events[-1]['passed'] and events[-1]['readsStarted'] == 2
print('HP: full journal parsed, two read-only requests, passed; manifest hashes match')
