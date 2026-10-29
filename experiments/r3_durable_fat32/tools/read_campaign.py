#!/usr/bin/env python3
"""Read a real .TR2 file using ordinary PC file I/O. Experimental format v1.
No raw disk I/O and no writes. Exit0 COMPLETE,2 INCOMPLETE,3 CORRUPT/unsupported.
Within-block times are linear interpolations of actual endpoints, not asserted
exact per-sample event timestamps. UTC is unknown; original XYZ/DC is retained.
"""
import argparse, hashlib, json, struct, zlib
from pathlib import Path
class Invalid(ValueError): pass
def require(ok, message):
    if not ok: raise Invalid(message)
def u32(b,o): return struct.unpack_from('<I',b,o)[0]
def u64(b,o): return struct.unpack_from('<Q',b,o)[0]
def verify(path):
    size=path.stat().st_size
    with path.open('rb') as stream:
        h=stream.read(512)
        require(len(h)==512 and h[:8]==b'TR2DUR01','header missing/unknown magic')
        require(u32(h,8)==1 and u32(h,12)==512,'unsupported version/header')
        require(u32(h,508)==zlib.crc32(h[:508]),'header CRC')
        run=u32(h,16);session=u64(h,152);num=u64(h,136);den=u64(h,144)
        require(3<=run<=5 and num and den and u32(h,124)==8 and u32(h,128)==244 and u32(h,132)==10 and u32(h,160)==78 and u32(h,164)==0,'unsupported profile')
        stream.seek(max(512,size-512));tail=stream.read(512);stream.seek(512)
        committed=tail[:8]==b'TR2END01'
        seq=count=flags=0;first=last=None;digest=hashlib.sha256();short=False
        stop_reason='missing footer or interrupted tail'
        while stream.tell()+512<=size:
            b=stream.read(512)
            if b[:8]==b'TR2END01':
                # Footer belongs at final sector of fixed preallocated file.
                require(stream.tell()==size,'footer misplaced')
                footer=b;break
            if b[:4]!=b'D3BK' or u64(b,8)!=session:
                stream.seek(max(512,size-512));footer=stream.read(512);break
            crc=u32(b,40);check=b[:40]+bytes(4)+b[44:]
            if crc!=zlib.crc32(check) and not committed:
                footer=tail;stop_reason='torn/corrupt tail record; valid prefix retained';break
            require(crc==zlib.crc32(check),'record CRC')
            n,quality=struct.unpack_from('<HH',b,36);start,end=u64(b,16),u64(b,24)
            require(not short and u32(b,4)==seq and u32(b,32)==count and 1<=n<=78,'record sequence/count')
            require(quality&~31==0 and not quality&24,'invalid time or counters')
            require(start<=end and (n==1 or start<end) and (last is None or start>last),'invalid timestamps')
            require(b[44+n*6:]==bytes(468-n*6),'record tail padding')
            digest.update(b[44:44+n*6]);first=start if first is None else first;last=end;count+=n;seq+=1;flags|=quality;short=n<78
        else: footer=b''
        complete=False
        if footer[:8]==b'TR2END01':
            require(len(footer)==512 and u32(footer,508)==zlib.crc32(footer[:508]),'footer CRC')
            require(u32(footer,8)==1 and u64(footer,16)==session and u32(footer,24)==run,'footer identity/version')
            require(u32(footer,12)==1 and u32(footer,48)==1 and u32(footer,52)==0,'footer not successful')
            require(u32(footer,28)==count and u32(footer,36)==seq and u32(footer,40)==count*6,'footer counts')
            require(count==(u32(footer,32)+9)//10,'source to output count')
            require((u64(footer,56),u64(footer,64))==(first,last),'footer time endpoints')
            require(footer[96:128]==digest.digest(),'upstream producer SHA mismatch')
            require(not u32(footer,76) and not u32(footer,80) and not flags&6,'rail or clipping in completed capture')
            complete=True;stop_reason='controlled STOP and full producer digest matched'
        return dict(status='COMPLETE' if complete else 'INCOMPLETE',path=str(path),file_bytes=size,run=run,session=session,valid_records=seq,valid_xyz=count,payload_bytes=count*6,sha256=digest.hexdigest(),first_output_source_index=0 if count else None,last_output_source_index=(count-1)*10 if count else None,first_time_us=first,last_time_us=last,source_rate_num=num,source_rate_den=den,source_hz=num/den,output_hz=num/den/10,logical_fs_g=8,sensitivity_ug_per_count=244,dc_preserved=True,quality_or=flags,utc=None,firmware_sha=h[52:92].decode('ascii'),coefficient_sha256=h[92:124].hex(),cid=h[36:52].hex(),uid=[u32(h,24+i*4) for i in range(3)],time_method='Actual DEC8 block endpoints; within-block interpolation approximate, not UTC',stop_reason=stop_reason,durability='CRC-valid sequential matching-session prefix only; physical survival after power loss and maximum lost tail are NOT guaranteed')
def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('file',type=Path);args=ap.parse_args()
    try:
        result=verify(args.file);print(json.dumps(result,indent=2));return 0 if result['status']=='COMPLETE' else 2
    except (Invalid,OSError,UnicodeError,struct.error) as e:
        print(json.dumps(dict(status='CORRUPT_OR_UNSUPPORTED',error=str(e)),indent=2));return 3
if __name__=='__main__':raise SystemExit(main())
