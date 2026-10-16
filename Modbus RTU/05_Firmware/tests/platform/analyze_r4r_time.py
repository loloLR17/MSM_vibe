"""Independent raw FIFO/counter oracle. Diagnostic evidence, no B3 authority.
Run --self-test before target, or directory containing raw.bin/brackets.bin.
Brackets are GDB dumps of little-endian ARM {u64,u64,u32,u32}.
"""
import argparse
import json
from pathlib import Path
import statistics
import struct

def fifo(data):
    assert len(data) % 7 == 0
    anchors=[]; index=0; last_tag=None; counter_errors=0; reserved_errors=0
    for (tag,x,y,z) in struct.iter_unpack('<BHHH',data):
        assert tag.bit_count()%2==0, ('parity',index,tag)
        counter=(tag>>1)&3
        if tag>>3==2:
            if last_tag is not None: assert counter==(last_tag+1)%4
            last_tag=counter; index+=1
        elif tag>>3==4:
            counter_errors+=counter!=last_tag
            reserved_errors+=z!=0x0a00
            anchors.append(dict(index=index-1,raw=x+(y<<16),counter=counter))
        else: raise AssertionError(('tag',tag))
    deltas=[(b['raw']-a['raw'])&0xffffffff for a,b in zip(anchors,anchors[1:])]
    assert all(0<d<0x80000000 for d in deltas)
    assert all(b['index']-a['index']==8 for a,b in zip(anchors,anchors[1:]))
    return dict(xyz=index,anchors=anchors,delta_ticks_min=min(deltas),delta_ticks_max=max(deltas),counter_mismatch=counter_errors,reserved_mismatch=reserved_errors)

def brackets(data):
    rows=list(struct.iter_unpack('<QQII',data)); assert len(rows)>2
    # Discard samples before the first sensor tick; startup and clock freeze
    # belong to distinct domains and are never folded into a rate estimate.
    rows=[r for r in rows if r[2]!=0]; first,last=rows[0],rows[-1]
    tickspan=(last[2]-first[2])&0xffffffff
    assert 0<tickspan<0x80000000
    mid=lambda r:(r[0]+r[1])/2
    us_per_tick=(mid(last)-mid(first))/tickspan
    residual=[mid(r)-mid(first)-((r[2]-first[2])&0xffffffff)*us_per_tick for r in rows]
    return dict(rows=len(rows),us_per_tick=us_per_tick,bracket_us_max=max(r[1]-r[0] for r in rows),residual_us_min=min(residual),residual_us_max=max(residual),first=first,last=last,method='endpoint slope; residuals observed, no guaranteed WCET or UTC',median_bracket_us=statistics.median(r[1]-r[0] for r in rows))

def self_test():
    words=[]
    for i in range(24):
        ctr=i%4; tag=16+2*ctr;tag|=tag.bit_count()%2
        words.append(struct.pack('<BHHH',tag,1,2,3))
        if i%8==7:
            tag=32+2*ctr;tag|=tag.bit_count()%2
            raw=(0xfffffff0+24*(i//8))&0xffffffff
            words.append(struct.pack('<BIH',tag,raw,0xa00))
    r=fifo(b''.join(words));assert r['delta_ticks_min']==24 and r['counter_mismatch']==0
    rows=b''.join(struct.pack('<QQII',10000+i*10000,10020+i*10000,800+i*800,i) for i in range(20))
    r=brackets(rows);assert r['us_per_tick']==12.5 and r['residual_us_max']==0
    bad=bytearray(b''.join(words));bad[0]^=1
    try: fifo(bad)
    except AssertionError: pass
    else: raise AssertionError('parity guard absent')
    print('oracle_self_test PASS: LE32 wrap, 8XYZ anchors, parity, 12.5us slope')

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--self-test',action='store_true');p.add_argument('directory',type=Path,nargs='?');a=p.parse_args()
    if a.self_test:self_test()
    if a.directory:
        r=dict(fifo_prefix=fifo((a.directory/'raw.bin').read_bytes()),clock=brackets((a.directory/'brackets.bin').read_bytes()))
        (a.directory/'time-analysis.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps(r,indent=2))
