from pathlib import Path
import hashlib,json
import hardware_second as h
P=Path(__file__).parent;old=Path('/home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/arm');expected=(old/'r3_coex.bin').read_bytes();assert hashlib.sha256(expected).hexdigest()=='b70438c7930ee2d928ec441a71a66ee23305d0fe5a102b2bc29e1453b6925330'
f=P/'hardware/preexisting-E6-readback.bin';assert not f.exists();path=h.win(f).replace('\\','/')
h.debug('preflight-identity-existing-E6-WAIT',old/'r3_coex.elf',h.IDENTITY+f'''p r3_result
p r3_clock_fault
if r3_result.version!=4 || r3_result.stage!=2 || r3_result.error || r3_clock_fault || r3_result.run || r3_command || fifo.running || r3_writer.active || !(*(unsigned int*)0x42021414 &0x1000)
 quit 5
end
dump binary memory {path} 0x08000000 0x{0x08000000+len(expected):x}
''')
assert f.read_bytes()==expected
(P/'PREFLIGHT.json').write_text(json.dumps(dict(SN=h.SN,UID='001B0046/38425018/20333157',old_BIN_exact=True,old_source='c3cd40ce9e96cef2241fb5bc6e8c83acd2a3824e',stage='WAIT',GDB_detached=True,no_reset_flash_SD_command=True),indent=2)+'\n')
print('Identity and entire previous E6 BIN exact; WAIT safe',flush=True)
