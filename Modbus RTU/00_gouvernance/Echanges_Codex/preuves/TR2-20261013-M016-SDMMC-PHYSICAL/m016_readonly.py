from pathlib import Path
import sys,time,hashlib
P=Path(Path('/tmp/m016-sd-physical-proof').read_text());sys.path.insert(0,str(P));from hardware import *
b=Path('/home/lolo/dev/msm/reports/M016-sdmmc-reference-build'); image=b/'sdmmc_reference.bin';assert hashlib.sha256(image.read_bytes()).hexdigest()=='b1091e8908614fb99576134b8cc18a334449073c3c9851fcc13e460e38e752f9'
flash('readonly-flash',image,'077835157fbdefecb4ebc2a416cbf82d1f8891d7')
debug('readonly-reset-readback',b/'sdmmc_reference.elf',IDENTITY+'dump binary memory "'+win(P/'readonly-flash-readback.bin')+'" 0x08000000 0x08004e8c\nmonitor reset\n')
assert (P/'readonly-flash-readback.bin').read_bytes()==image.read_bytes()
time.sleep(5)
debug('readonly-preflight',b/'sdmmc_reference.elf',IDENTITY+'''p bench_stage
p bench_phase
p/x bench_error
p/x bench_cid
p bench_blocks
p bench_kernel_hz
p bench_clock_hz
p/x bench_clkcr
p/x bench_bus_width
p/x bench_last_r1
p/x bench_cmd13_error
p hsd2.SdCard
p/x SDMMC2->CLKCR
x/2wx 0xE000ED28
''')
print((P/'readonly-preflight.log').read_text(errors='replace'))
