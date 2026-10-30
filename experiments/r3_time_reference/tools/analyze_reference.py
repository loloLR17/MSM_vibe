"""Bounds between request-send/response-receive QPC stamps, not UTC."""
import argparse,json,struct,zlib
from fractions import Fraction
from pathlib import Path
UID=(0x001b0046,0x38425018,0x20333157)
def frame(raw):
    if len(raw)!=32 or raw[:4]!=b'QPC1' or zlib.crc32(raw[:28])!=struct.unpack_from('<I',raw,28)[0]:raise ValueError('frame CRC/magic/length')
    uid=struct.unpack_from('<3I',raw,4)
    if uid!=UID:raise ValueError('wrong target UID')
    seq,cycles=struct.unpack_from('<IQ',raw,16)
    return seq,cycles

def bounds(data):
    f=int(data['frequency']);a,b=data['queries']
    if not data['high_resolution'] or f<=0:raise ValueError('QPC unavailable')
    s0,e0,s1,e1=map(int,[a['send_tick'],a['receive_tick'],b['send_tick'],b['receive_tick']])
    if not 0<=s0<=e0<s1<=e1:raise ValueError('non-monotone/overlapping brackets')
    qa,ca=frame(bytes.fromhex(a['frame_hex']));qb,cb=frame(bytes.fromhex(b['frame_hex']))
    if qb!=qa+1 or cb<=ca:raise ValueError('reset/replay/cycles')
    low,high=Fraction(s1-e0,f),Fraction(e1-s0,f);cycles=cb-ca
    return dict(reference='Windows QPC independent of STM32; not SI traceable',elapsed_reference_s_bounds=[float(low),float(high)],cycles=cycles,core_Hz_bounds=[float(cycles/high),float(cycles/low)],nominal_160MHz_inside_bounds=cycles/high<=160000000<=cycles/low,retroactive_hour_qualified=False,source='Short idle probe only; no FIFO/SD or hour captured')

if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('file',type=Path);a=ap.parse_args();print(json.dumps(bounds(json.loads(a.file.read_text(encoding='utf-8-sig'))),indent=2))
