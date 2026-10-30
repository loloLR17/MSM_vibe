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


def active_frame(raw):
    if len(raw)!=48 or raw[:4]!=b'QPC2' or zlib.crc32(raw[:44])!=struct.unpack_from('<I',raw,44)[0]:raise ValueError('active frame CRC/version')
    if struct.unpack_from('<3I',raw,4)!=UID:raise ValueError('wrong target UID')
    seq,us,flags,session,accepted=struct.unpack_from('<IQIQI',raw,16)
    return dict(sequence=seq,mcu_us=us,flags=flags,session=session,accepted=accepted)

def active_bounds(data):
    if data.get('failure') or not data.get('high_resolution'):raise ValueError('failed witness')
    a,b=data['first'],data['second'];x,y=active_frame(bytes.fromhex(a['frame_hex'])),active_frame(bytes.fromhex(b['frame_hex']))
    f=int(data['frequency']);n=int(data['minimum_seconds']);run=int(data['expected_run'])
    if f<=0 or n<1 or not 0<=int(a['send_tick'])<=int(a['receive_tick'])<int(b['send_tick'])<=int(b['receive_tick']):raise ValueError('clock brackets')
    if any(v['flags']!=(1|(run<<8)) or not v['accepted'] or not v['session'] for v in [x,y]):raise ValueError('not active capture')
    if x['session']!=y['session'] or y['sequence']!=x['sequence']+1 or y['mcu_us']<=x['mcu_us'] or y['accepted']<=x['accepted']:raise ValueError('session/replay/source/time')
    delta=int(b['send_tick'])-int(a['receive_tick'])
    if delta<n*f+2:raise ValueError('insufficient inner interval')
    return dict(reference='Windows QPC, not UTC/SI traceability',independent_inner_interval_s=float(Fraction(delta,f)),minimum_required_s=n,run=run,session=x['session'],first=x,second=y,continuous_successful_file_still_required=True,physical_hour_gate_qualified=False)

if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('file',type=Path);a=ap.parse_args();data=json.loads(a.file.read_text(encoding='utf-8-sig'));print(json.dumps(active_bounds(data) if data.get('mode')=='ACTIVE_INNER_INTERVAL_QPC2' else bounds(data),indent=2))
