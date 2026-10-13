import hardware as h
from pathlib import Path
import time,subprocess,hashlib
base=h.P/'qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE'
bin=Path(str(base)+'.bin');elf=Path(str(base)+'.elf')
assert hashlib.sha256(bin.read_bytes()).hexdigest()=='00ba4f5b6750f3c20b2bd18832c99eaf10ac257449df6df12a53f6500ed81e5d'
sha=subprocess.check_output(['git','rev-parse','HEAD'],text=True).strip()
h.flash('flash-storage-attempt02',bin,sha)
h.debug('attempt02-reset-and-flash-readback',elf,h.IDENTITY+'dump binary memory '+h.win(h.P/'attempt02-flash-readback.bin')+f' 0x08000000 0x{0x08000000+bin.stat().st_size:08X}\nmonitor reset\n')
assert (h.P/'attempt02-flash-readback.bin').read_bytes()==bin.read_bytes()
time.sleep(30)
h.debug('attempt02-prewrite-gate',elf,h.IDENTITY+"""p storage_qual_stage
p storage_qual_result
p storage_qual_scan_nonzero
p storage_qual_recovery_status
p storage_qual_recovery_bytes
p tr2_commission_stage
p tr2_commission_fram_result
p tr2_commission_verify_result
p hsd2.CID
p hsd2.ErrorCode
""")
print((h.P/'attempt02-prewrite-gate.log').read_text())
