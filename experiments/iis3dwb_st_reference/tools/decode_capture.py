#!/usr/bin/env python3
"""Decode a populated RAM FIFO dump offline, retaining raw bytes and diagnostics."""
import argparse
import csv
import hashlib
import json
from pathlib import Path
import struct

def decode(data):
    if len(data) % 7 or len(data) > 512 * 7:
        raise ValueError('Expected only count*7 populated bytes, at most 512 entries')
    rows = []
    for index in range(0, len(data), 7):
        word = data[index:index + 7]
        tag = word[0] >> 3
        row = {'index': index // 7, 'raw_hex': word.hex(), 'tag_byte': word[0],
               'sensor': tag, 'counter': (word[0] >> 1) & 3,
               'parity_even': word[0].bit_count() % 2 == 0,
               'x': '', 'y': '', 'z': '', 'timestamp_ticks': '', 'accepted': False}
        if tag == 2:
            row['x'], row['y'], row['z'] = struct.unpack('<hhh', word[1:])
        elif tag == 4:
            row['timestamp_ticks'] = struct.unpack('<I', word[1:5])[0]
        rows.append(row)
    return rows

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('dump', type=Path)
    parser.add_argument('--csv', type=Path, required=True)
    args = parser.parse_args()
    data = args.dump.read_bytes()
    rows = decode(data)
    with args.csv.open('x', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=list(rows[0]) if rows else
                                ['index', 'raw_hex', 'accepted'])
        writer.writeheader(); writer.writerows(rows)
    print(json.dumps({'sha256': hashlib.sha256(data).hexdigest(), 'received_entries': len(rows),
                      'qualification': 'NOT_ESTABLISHED', 'csv': str(args.csv)}))

if __name__ == '__main__':
    main()
