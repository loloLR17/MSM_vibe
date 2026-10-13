from pathlib import Path
import sys,time
P=Path(Path('/tmp/m016-sd-physical-proof').read_text());sys.path.insert(0,str(P));from hardware import *
b=Path('/home/lolo/dev/msm/reports/M016-sdmmc-reference-build')
debug('readonly-readback-reset-corrected',b/'sdmmc_reference.elf',IDENTITY+'dump binary memory '+win(P/'readonly-flash-readback.bin')+' 0x08000000 0x08004e8c\nmonitor reset\n')
assert (P/'readonly-flash-readback.bin').read_bytes()==(b/'sdmmc_reference.bin').read_bytes();time.sleep(3)
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
x/2wx 0xE000ED28
''')
print((P/'readonly-preflight.log').read_text(errors='replace'))
