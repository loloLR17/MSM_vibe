"""Offline replay of the rejected M015-B capture; never accesses hardware."""
from pathlib import Path
import csv
import hashlib
import json
import struct
p = Path(__file__).resolve().parent
info = json.loads((p / 'fifo-info.json').read_text())
a = json.loads((p / 'fifo-analysis.json').read_text())
with (p / 'fifo-status.csv').open() as f:
    rows = [[int(row[k]) for k in ('begin_cycles', 'end_cycles', 'raw_status', 'level')]
            for row in csv.DictReader(f)]
assert len(rows) == info['poll_count'] == 171
binary = b''.join(struct.pack('<4I', *row) for row in rows)
assert hashlib.sha256(binary).hexdigest() == a['polls_prefix_sha256']
assert sum(b[3] < x[3] for x, b in zip(rows, rows[1:])) == len(a['level_decreases']) == 11
assert rows[169][2] == 0x40FE and rows[170][2] == 0xA100
assert info['count'] == 0 and info['result'] == 2 and info['loss_flags'] == 33
assert info['configured'][1] == 0x80
assert info['restore_result'] == 0 and info['restored'] == info['saved']
with (p / 'hp-restored-physical.jsonl').open() as f:
    events = [json.loads(line) for line in f]
assert [(x['start'], x['count']) for x in events if x['event'] == 'read_intent'] == [(0, 21), (1000, 20)]
assert events[-1]['passed'] and events[-1]['readsStarted'] == 2
assert hashlib.sha256((p / 'hp-restored-physical.jsonl').read_bytes()).hexdigest() == json.loads((p / 'hp-restored-analysis.json').read_text())['journal_sha256']
print('Rejected capture reproduced: 171 polls, 11 level drops, zero accepted XYZ; M014 two-read recovery passed')
