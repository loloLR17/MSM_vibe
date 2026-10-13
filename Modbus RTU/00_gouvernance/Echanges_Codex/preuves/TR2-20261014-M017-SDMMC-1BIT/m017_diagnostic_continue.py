from pathlib import Path
import sys
P=Path('/home/lolo/dev/msm/reports/TR2-M017-20261014');sys.path.insert(0,str(P));from hardware import *
elf=Path('/home/lolo/dev/msm/reports/M017-diagnostic-build/sdmmc_reference.elf')
debug('diagnostic-preflight',elf,IDENTITY+'''p load_stage
p load_error
p/x load_cid
p load_blocks
p load_clock_hz
p load_clkcr
p load_bus_card
p/x GPIOB->MODER
if load_stage != 2 || load_error != 0 || load_clock_hz != 10000000 || load_bus_card != 0
 quit 4
end
''')
debug('diagnostic-differential',elf,IDENTITY+'''hbreak differential
set load_trigger=0x4D313744
continue
finish
p diag_done
p diag_error
p diag_mismatches
p diag_latency_us
p diag_data_us
p diag_ready_us
p/x hsd2.ErrorCode
p/x load_last_r1
x/2wx 0xE000ED28
''',timeout=80)
print((P/'diagnostic-differential.log').read_text())
