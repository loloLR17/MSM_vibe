import hardware as h
from pathlib import Path
import time,subprocess,hashlib
base=h.P/'qualification-build/tr2-special-storage_qualification-NON-CUMULATIVE'
bin=Path(str(base)+'.bin');elf=Path(str(base)+'.elf')
assert hashlib.sha256(bin.read_bytes()).hexdigest()=='d1b736a8027fbd373c4ce36c15a0a9f4d7a2d882a92e876e1ea95a24fb09b463'
sha=subprocess.check_output(['git','rev-parse','HEAD'],text=True).strip()
h.flash('flash-storage-qualification',bin,sha)
h.debug('qualification-reset-and-flash-readback',elf,h.IDENTITY+'dump binary memory '+h.win(h.P/'qualification-flash-readback.bin')+f' 0x08000000 0x{0x08000000+bin.stat().st_size:08X}\nmonitor reset\n')
assert (h.P/'qualification-flash-readback.bin').read_bytes()==bin.read_bytes()
time.sleep(30)
h.debug('qualification-prewrite-gate',elf,h.IDENTITY+"""p storage_qual_stage
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
print((h.P/'qualification-prewrite-gate.log').read_text())
