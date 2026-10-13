from pathlib import Path
import sys, hashlib, subprocess,json
P=Path('/home/lolo/dev/msm/reports/TR2-M017-20261014');sys.path.insert(0,str(P));from hardware import *
old=Path('/home/lolo/dev/msm/reports/M016-1bit-load-build/sdmmc_reference.elf')
debug('preflight-old-safe',old,IDENTITY+'p load_stage\np load_error\np/x load_cid\np load_blocks\np load_clock_hz\nx/wx 0x42020400\nx/wx 0x420C0400\nx/wx 0x420C0404\n')
image=Path('/home/lolo/dev/msm/reports/M017-diagnostic-build/sdmmc_reference.bin');elf=image.with_suffix('.elf')
assert image.stat().st_size<32768
flash('diagnostic-flash',image,'uncommitted diagnostic snapshot; diff saved')
debug('diagnostic-reset-verify',elf,IDENTITY+'monitor reset\ndump binary memory '+win(P/'diagnostic-flash-readback.bin')+' 0x08000000 '+hex(0x08000000+image.stat().st_size)+'\n')
assert image.read_bytes()==(P/'diagnostic-flash-readback.bin').read_bytes()
debug('diagnostic-init',elf,IDENTITY+'set load_trigger=0x4C31494E\ncontinue &\nshell sleep 1\ninterrupt\np load_stage\np load_error\np/x load_cid\np load_blocks\np load_clock_hz\np load_clkcr\np load_bus_card\n')
# Break once when differential returns, avoiding any pause during transfer.
debug('diagnostic-differential',elf,IDENTITY+'''if load_stage != 2 || load_error != 0 || diag_done != 0
 quit 4
end
hbreak differential
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
