"""Six-hour metadata and quality-degraded salvage; fixture, not physical data."""
from pathlib import Path
import hashlib
import json
import struct
import sys
import tempfile
import zlib

sys.path.insert(0, str(Path(__file__).parents[1] / 'tools'))
import read_campaign as reader

fixture = Path(sys.argv[1]).read_bytes()
header = bytearray(fixture[:512])
record = bytearray(fixture[512:1024])
struct.pack_into('<I', header, 8, 3)
struct.pack_into('<I', header, 20, 0)
struct.pack_into('<Q', header, 168, 22000000000)
struct.pack_into('<I', header, 508, zlib.crc32(header[:508]))
struct.pack_into('<H', record, 38, reader.u32(record, 36) >> 16 | 4)
struct.pack_into('<h', record, 44 + 10 * 6, 32767)
struct.pack_into('<I', record, 40, 0)
struct.pack_into('<I', record, 40, zlib.crc32(record))
count = struct.unpack_from('<H', record, 36)[0]
# Producer bytes/digest preserved independently before writing this fixture.
producer_bytes = bytes(record[44:44 + count * 6])
producer_sha = hashlib.sha256(producer_bytes).hexdigest()
with tempfile.TemporaryDirectory() as directory:
    path = Path(directory) / 'E6N0005.TR2'
    with path.open('xb') as stream:
        stream.write(header)
        stream.write(record)
        stream.truncate(640 * 1024 * 1024)  # No success footer, zero unused tail.
    result = reader.verify(path)
    assert result['status'] == 'INCOMPLETE'
    assert result['requested_duration_us'] == 22000000000
    assert result['valid_xyz'] == count and result['valid_records'] == 1
    assert result['payload_bytes'] == len(producer_bytes)
    assert result['quality_or'] & 4 and result['sha256'] == producer_sha
    assert result['uid'] == list(struct.unpack_from('<3I', header, 24))
    assert result['session'] == reader.u64(header, 152)
    # Torn record must never be counted as an intact sample prefix.
    with path.open('r+b') as stream:
        stream.seek(512 + 100)
        stream.write(bytes([record[100] ^ 1]))
    torn = reader.verify(path)
    assert torn['status'] == 'INCOMPLETE' and torn['valid_xyz'] == 0
print(json.dumps(dict(six_hour_u64_metadata=True, clipped_prefix_salvaged=True,
                      source_digest_external=True, false_COMPLETE=False,
                      torn_record_not_counted=True, host_fixture_only=True)))
