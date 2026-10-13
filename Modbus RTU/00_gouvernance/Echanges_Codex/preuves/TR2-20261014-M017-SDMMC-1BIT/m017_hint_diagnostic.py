from pathlib import Path
import sys,time,hashlib,subprocess,json
P=Path('/home/lolo/dev/msm/reports/TR2-M017-20261014');sys.path.insert(0,str(P));from hardware import *
b=Path('/home/lolo/dev/msm/reports/M017-diagnostic-build');elf=b/'sdmmc_reference.elf';image=b/'sdmmc_reference.bin'
flash('hint-diagnostic-flash',image,'65ff1fa + isolated ACMD23 experiment; source diff archived')
debug('hint-diagnostic-readback',elf,IDENTITY+'dump binary memory '+win(P/'hint-diagnostic-image.bin')+' 0x08000000 '+hex(0x08000000+image.stat().st_size)+'\nmonitor reset\n')
assert image.read_bytes()==(P/'hint-diagnostic-image.bin').read_bytes();time.sleep(2)
debug('hint-diagnostic-init',elf,IDENTITY+'if load_stage != 1\n quit 4\nend\nset load_trigger=0x4C31494E\n');time.sleep(2)
source=Path('experiments/sdmmc_st_reference/src/load_main.c').read_text().splitlines();line=next(i+1 for i,t in enumerate(source) if t.strip()=='polling_transport=0;')
debug('hint-diagnostic-differential',elf,IDENTITY+f'''if load_stage != 2 || load_error != 0 || load_clock_hz != 10000000 || load_bus_card != 0 || diag_done != 0
 quit 4
end
hbreak load_main.c:{line}
set load_trigger=0x4D313744
continue
p diag_done
p diag_error
p diag_mismatches
p diag_latency_us
p diag_data_us
p diag_ready_us
p load_preerase_count
p/x load_preerase_error
p/x load_preerase_r1
p/x hsd2.ErrorCode
p/x load_cid
p/x load_clkcr
p load_clock_hz
''',timeout=80)
print((P/'hint-diagnostic-differential.log').read_text())
