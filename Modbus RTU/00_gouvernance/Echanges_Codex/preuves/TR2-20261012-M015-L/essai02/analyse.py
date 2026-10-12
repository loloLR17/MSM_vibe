from pathlib import Path
import json,struct,csv,hashlib
p=Path(__file__).parent
keys=['tick','reg','len','write','hal_status','hal_error','rx_left_after_HAL','tx_left_after_HAL','offset','valid']
results={}
for stage in ['A','B','C','C2']:
 name='stage-'+stage;b=(p/(name+'-traces.bin')).read_bytes();payload=(p/(name+'-payload.bin')).read_bytes();rows=[]
 for i in range(0,len(b),40):
  row=dict(zip(keys,struct.unpack('<10I',b[i:i+40])));row['index']=i//40;row['command_from_code']=f"{row['reg'] if row['write'] else row['reg']|0x80:02X}";row['buffer_hex']=payload[row['offset']:row['offset']+row['len']].hex();rows.append(row)
 with (p/(name+'-traces.csv')).open('w',newline='') as f:
  w=csv.DictWriter(f,fieldnames=list(rows[0]));w.writeheader();w.writerows(rows)
 original=json.loads((p/(name+'-analysis.json')).read_text());item={k:v for k,v in original.items() if k!='xyz'};item['attempt_id']=2;item['software_sha']='e222dc8ddb7cfd0829c655533ac8e646220721f2';item['first_failed_transfer']=next((r for r in rows if not r['valid']),None)
 if stage=='A':item.update({'trial':'WHO essai n°2 après correction du câblage','payload':payload.hex(),'sensor_writes':sum(r['write'] for r in rows)})
 if stage=='B':
  reads=[r for r in rows if r['reg']==0x28 and r['len']==6 and not r['write']];assert len(reads)==64;raw=b''.join(bytes.fromhex(r['buffer_hex']) for r in reads)
  snap=(p/(name+'-result.bin')).read_bytes();assert raw==snap[3646:4030]
  xyz=original['xyz'];assert all(tuple(v)==struct.unpack('<3h',raw[i*6:(i+1)*6]) for i,v in enumerate(xyz))
  (p/'polling-xyz.bin').write_bytes(raw)
  with (p/'polling-xyz.csv').open('w',newline='') as f:
   w=csv.writer(f);w.writerow(['index','tick_mcu_ms_not_conversion_timestamp','raw_hex','x_lsb','y_lsb','z_lsb'])
   for i,(r,v) in enumerate(zip(reads,xyz)):w.writerow([i,r['tick'],r['buffer_hex'],*v])
  item['raw_xyz_sha256']=hashlib.sha256(raw).hexdigest();item['raw_reconstructed_exact']=True;item['ranges_lsb']={axis:[min(row[i] for row in xyz),max(row[i] for row in xyz)] for i,axis in enumerate(['x','y','z'])};item['distinct_per_axis']=[len(set(row[i] for row in xyz)) for i in range(3)]
 if stage in ['C','C2']:
  statuses=[{'trace':r['index'],'tick':r['tick'],'level_raw':int.from_bytes(bytes.fromhex(r['buffer_hex']),'little')&0x3FF,'status_hex':r['buffer_hex'],'valid':r['valid']} for r in rows if r['reg']==0x3A and r['len']==2 and not r['write']]
  (p/(name+'-status.json')).write_text(json.dumps(statuses,indent=2)+'\n');item['status_observations']=len(statuses);item['max_observed_raw_level']=max(v['level_raw'] for v in statuses);item['flags_failure_observed_before_drain']=any((int.from_bytes(bytes.fromhex(v['status_hex']),'little')&0x6800) for v in statuses);item['post_final_transfer_status_not_observed']=True
 if stage=='C2':
  failed=item['first_failed_transfer'];assert failed['reg']==0x78 and failed['len']==896 and failed['hal_status']==3 and failed['hal_error']==0x104
  buf=bytes.fromhex(failed['buffer_hex']);snap=(p/(name+'-result.bin')).read_bytes();assert buf==snap[62:62+896]
  (p/'fifo-failed-buffer.bin').write_bytes(buf);item['failed_buffer_sha256']=hashlib.sha256(buf).hexdigest();item['buffer_bytes']=896;item['real_received_count']='NOT_RECORDED';item['buffer_initially_zeroed']=True;item['all_slots_rejected']=True;item['zero_tail_start_after_last_nonzero_byte']=max(i for i,v in enumerate(buf) if v)+1;item['spi_error_bits']=['SPI_OVR 0x04','TIMEOUT 0x100'];item['HAL_clears_remaining_counts_on_close']=True;item['capacity_not_exhausted']={'trace_count':len(rows),'trace_limit':1024,'payload_bytes':len(payload),'payload_limit':65536}
  # No CSV claiming 128 received samples: failure/zeroed tail makes length unknown.
 results[stage]=item
(p/'analysis-summary.json').write_text(json.dumps(results,indent=2)+'\n')
print(json.dumps({s:{k:v for k,v in r.items() if k in ['result','cleanup_result','who','count','distinct_xyz','ranges_lsb','max_observed_raw_level','first_failed_transfer','real_received_count','capacity_not_exhausted']} for s,r in results.items()},indent=2))
