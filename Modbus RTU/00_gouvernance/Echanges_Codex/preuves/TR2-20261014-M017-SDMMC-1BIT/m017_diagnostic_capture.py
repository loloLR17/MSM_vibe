from pathlib import Path
import sys
P=Path('/home/lolo/dev/msm/reports/TR2-M017-20261014');sys.path.insert(0,str(P));from hardware import *
elf=Path('/home/lolo/dev/msm/reports/M017-diagnostic-build/sdmmc_reference.elf')
debug('diagnostic-final',elf,IDENTITY+'''p load_stage
p diag_done
p diag_error
p diag_mismatches
p diag_latency_us
p diag_data_us
p diag_ready_us
p/x hsd2.ErrorCode
p/x load_last_r1
x/2wx 0xE000ED28
''')
print((P/'diagnostic-final.log').read_text())
