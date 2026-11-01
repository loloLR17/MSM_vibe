from pathlib import Path
import struct,json
P=Path(__file__).parent;d=P/'n8e-card-calibrate';raw=(d/'fifo-stop-trace.bin').read_bytes();payload=(d/'fifo-stop-payload.bin').read_bytes();assert len(raw)%40==0
records=[]
for i,row in enumerate(struct.iter_unpack('<10I',raw)):
 tick,reg,n,write,status,error,rx,tx,offset,valid=row;b=payload[offset:offset+n];assert len(b)==n
 records.append(dict(index=i,tick=tick,reg=hex(reg),length=n,write=bool(write),status=status,error=error,rx_remaining=rx,tx_remaining=tx,valid=bool(valid),payload=b.hex()))
(P/'CONFIG_TRANSACTIONS.json').write_text(json.dumps(records,indent=2)+'\n')
for r in records:print(r['index'],'W' if r['write'] else 'R',r['reg'],r['length'],r['payload'],'ok=',r['valid'])
