#!/usr/bin/env python3
"""Independent offline checks; no hardware or dependency on bench decode_capture."""
import struct

def analyze(data, *, expected_entries, result, cleanup, transfers, statuses,
            config_ok=True, trace_refused=0, timestamp_decimation=8):
    reasons=[]
    if len(data)%7: raise ValueError("Partial FIFO word")
    rows=[];previous_counter=None;counter_errors=0;parity_errors=0;unknown=0
    timestamps=[];xyz=[];between=[];since_ts=0
    for i in range(0,len(data),7):
        word=data[i:i+7];tag=word[0]//8;counter=(word[0]//2)%4
        parity=(word[0].bit_count()%2)==0
        if not parity:parity_errors+=1
        row={'index':i//7,'raw_hex':word.hex(),'tag':tag,'counter':counter,'parity':parity}
        if tag==2:
            value=struct.unpack('<hhh',word[1:]);xyz.append(value);row['xyz']=value
            if previous_counter is not None and counter!=(previous_counter+1)%4:counter_errors+=1
            previous_counter=counter;since_ts+=1
        elif tag==4:
            ts=int.from_bytes(word[1:5],'little');row['timestamp']=ts;timestamps.append(ts)
            if len(timestamps)>1:between.append(since_ts)
            since_ts=0
        else:unknown+=1
        rows.append(row)
    deltas=[(b-a)%2**32 for a,b in zip(timestamps,timestamps[1:])]
    if len(rows)!=expected_entries:reasons.append('partial_window')
    if result:reasons.append('firmware_refusal')
    if cleanup:reasons.append('cleanup_failed')
    if trace_refused:reasons.append('logging_capacity')
    if not config_ok:reasons.append('config_refusal')
    if not transfers or any(t['status'] or t['error'] or not t['valid'] for t in transfers):reasons.append('SPI_refusal')
    if not statuses or any(s&0x68 for s in statuses):reasons.append('FIFO_overflow_or_missing_status')
    if not xyz:reasons.append('no_XYZ')
    if unknown:reasons.append('unknown_tag')
    if parity_errors:reasons.append('tag_parity')
    if counter_errors:reasons.append('counter_discontinuity')
    if timestamp_decimation and not timestamps:reasons.append('missing_timestamp')
    if any(d==0 or d>=2**31 for d in deltas):reasons.append('timestamp_nonmonotonic')
    if xyz and timestamp_decimation and any(n!=timestamp_decimation for n in between):reasons.append('timestamp_XYZ_ratio')
    return {'entries':len(rows),'xyz_count':len(xyz),'timestamp_count':len(timestamps),
            'parity_errors':parity_errors,'counter_discontinuities':counter_errors,
            'timestamp_deltas':deltas,'XYZ_between_timestamps':between,
            'xyz_distinct':len(set(xyz)),'saturated_components':sum(v in (-32768,32767) for row in xyz for v in row),
            'reasons':reasons,'bounded_acceptance':not reasons,'global_loss_zero_proven':False,'rows':rows}
