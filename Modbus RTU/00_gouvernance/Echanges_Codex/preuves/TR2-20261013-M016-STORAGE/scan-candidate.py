import hardware as h
from pathlib import Path
import hashlib,json
elf=h.P/'readonly-build/tr2-special-commissioning_readonly-NON-CUMULATIVE.elf'
body=h.IDENTITY
for index in range(1,256):
    lba=16384+128*index
    body+=f"set variable tr2_commission_sd_read_lba={lba}\nset variable tr2_commission_sd_read_blocks=128\nset variable tr2_commission_sd_read_trigger=0x4D313653\nthbreak CommissioningReadChunk\ncontinue\nfinish\nif tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != {index+1}\n quit 4\nend\n"
    body+='dump binary memory '+h.win(h.P/f'candidate-{index:03d}.bin')+' tr2_commission_sd_chunk tr2_commission_sd_chunk+65536\n'
h.debug('candidate-complete-scan',elf,body,timeout=300)
blocks=[h.P/'candidate-first64k.bin']+[h.P/f'candidate-{i:03d}.bin' for i in range(1,256)]
data=b''.join(f.read_bytes() for f in blocks)
assert len(data)==16777216
out={'start_lba':16384,'blocks':32768,'size_bytes':len(data),'sha256':hashlib.sha256(data).hexdigest(),'nonzero_bytes':sum(x!=0 for x in data),'all_zero':not any(data),'chunks':[{'index':i,'nonzero_bytes':sum(x!=0 for x in f.read_bytes()),'sha256':hashlib.sha256(f.read_bytes()).hexdigest()} for i,f in enumerate(blocks)],'reads_only':True,'allocated':False}
h.P.joinpath('candidate-scan.json').write_text(json.dumps(out,indent=2)+'\n');h.P.joinpath('candidate-complete-backup.bin').write_bytes(data)
print('Candidate16MiB full scan:',out['sha256'],'nonzero',out['nonzero_bytes'])
