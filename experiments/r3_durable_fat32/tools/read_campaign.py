#!/usr/bin/env python3
"""Read a real .TR2 file using ordinary PC file I/O. Experimental formats v1/v2/v3 (v3 has explicit64-bit durations).
No raw disk I/O and no writes. Exit0 COMPLETE,2 INCOMPLETE,3 CORRUPT/unsupported.
Within-block times are linear interpolations of actual endpoints, not asserted
exact per-sample event timestamps. UTC is unknown; original XYZ/DC is retained.
"""
import argparse, csv, hashlib, json, struct, zlib
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
        require(u32(h,8) in (1,2,3) and u32(h,12)==512,'unsupported version/header')
        require(u32(h,508)==zlib.crc32(h[:508]),'header CRC')
        version=u32(h,8);requested=u64(h,168) if version==3 else u32(h,20)
        if version==3:require(u32(h,20)==0 and 0<requested<=86400000000 and h[176:508]==bytes(332),'v3 requested/reserved fields')
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
            require(u32(footer,8)==u32(h,8) and u64(footer,16)==session and u32(footer,24)==run,'footer identity/version')
            require(u32(footer,12)==1 and u32(footer,48)==1 and u32(footer,52)==0,'footer not successful')
            require(u32(footer,28)==count and u32(footer,36)==seq and u32(footer,40)==count*6,'footer counts')
            require(count==(u32(footer,32)+9)//10,'source to output count')
            require((u64(footer,56),u64(footer,64))==(first,last),'footer time endpoints')
            require(footer[96:128]==digest.digest(),'upstream producer SHA mismatch')
            require(not u32(footer,76) and not u32(footer,80) and not flags&6,'rail or clipping in completed capture')
            if version>=2:
                actual=u64(footer,144) if version==3 else u32(footer,44)
                if version==3:require(u32(footer,44)==0 and footer[152:508]==bytes(356),'v3 footer reserved fields')
                require(requested<=actual<=requested+5000,'requested duration not completed')
                require(u64(footer,128)==u32(footer,88)+((u64(footer,128)>>32)<<32) and u64(footer,136)==u32(footer,92)+((u64(footer,136)>>32)<<32),'source endpoint low word')
                require(u64(footer,128)==first and u64(footer,136)>=last and u64(footer,136)>=u64(footer,128),'source endpoints')
            complete=True;stop_reason='controlled STOP and full producer digest matched'
        return dict(format_version=u32(h,8),requested_duration_us=requested,actual_duration_us=(u64(footer,144) if version==3 else u32(footer,44)) if complete else None,first_source_time_us=(u64(footer,128) if version>=2 else u32(footer,88)) if complete else None,last_source_time_us=(u64(footer,136) if version>=2 else u32(footer,92)) if complete else None,status='COMPLETE' if complete else 'INCOMPLETE',path=str(path),file_bytes=size,run=run,session=session,valid_records=seq,valid_xyz=count,payload_bytes=count*6,sha256=digest.hexdigest(),first_output_source_index=0 if count else None,last_output_source_index=(count-1)*10 if count else None,first_time_us=first,last_time_us=last,source_rate_num=num,source_rate_den=den,source_hz=num/den,output_hz=num/den/10,clock_ratio_scope='initial sensor/MCU calibration',observed_duration_us=(last-first if first is not None else None),observed_output_hz_between_endpoints=((count-1)*1000000/(last-first) if count>1 and last>first else None),logical_fs_g=8,sensitivity_ug_per_count=244,dc_preserved=True,quality_or=flags,utc=None,firmware_sha=h[52:92].decode('ascii'),coefficient_sha256=h[92:124].hex(),cid=h[36:52].hex(),uid=[u32(h,24+i*4) for i in range(3)],time_method='Actual DEC8 block endpoints; within-block interpolation approximate, not UTC',stop_reason=stop_reason,durability='CRC-valid sequential matching-session prefix only; physical survival after power loss and maximum lost tail are NOT guaranteed')
def samples(path):
    """Yield verified-prefix XYZ with approximate block-interpolated event time.
    Call verify first; quality is an aggregate per block except FIR warmup, whose
    causal phase is known (output indices0..10). No UTC or physical calibration.
    """
    result=verify(path)
    with path.open('rb') as stream:
        stream.seek(512)
        for _ in range(result['valid_records']):
            b=stream.read(512);n,quality=struct.unpack_from('<HH',b,36);first,last=u64(b,16),u64(b,24);index=u32(b,32)
            for j in range(n):
                x,y,z=struct.unpack_from('<hhh',b,44+j*6);i=index+j
                yield dict(output_index=i,source_index=10*i,time_us_interpolated=first+(last-first)*j//max(n-1,1),block_first_time_us=first,block_last_time_us=last,x_count=x,y_count=y,z_count=z,x_nominal_g=format(x*.000244,'.6f'),y_nominal_g=format(y*.000244,'.6f'),z_nominal_g=format(z*.000244,'.6f'),warmup=int(i<11),block_quality_flags=quality)
def write_csv(path,destination):
    rows=iter(samples(path));first=next(rows,None)
    if first is None: raise Invalid('no valid samples to export')
    # Exclusive creation preserves existing exports. The caller chooses a PC
    # destination; qualification uses an external report directory, never F:.
    with destination.open('x',newline='',encoding='utf-8') as output:
        writer=csv.DictWriter(output,fieldnames=list(first));writer.writeheader();writer.writerow(first);writer.writerows(rows)
def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('file',type=Path);ap.add_argument('--csv',type=Path,help='new CSV on PC; times interpolated, g values nominal');args=ap.parse_args()
    try:
        result=verify(args.file)
        if args.csv: write_csv(args.file,args.csv)
        print(json.dumps(result,indent=2));return 0 if result['status']=='COMPLETE' else 2
    except (Invalid,OSError,UnicodeError,struct.error) as e:
        print(json.dumps(dict(status='CORRUPT_OR_UNSUPPORTED',error=str(e)),indent=2));return 3
if __name__=='__main__':raise SystemExit(main())
