from pathlib import Path
import sys,hashlib,json,shutil,subprocess
P=Path('/home/lolo/dev/msm/reports/TR2-M017-20261014');sys.path.insert(0,str(P));from hardware import *
B=Path('/home/lolo/dev/msm/reports/M017-diagnostic-build')
for ext in ['elf','bin','map']:shutil.copyfile(B/f'sdmmc_reference.{ext}',P/f'hint-image.{ext}')
# Prefix was written before the later verified flash + software reset + card INIT.
# Read once again only to qualify this distinct, contract-requested persistence boundary.
elf=B/'sdmmc_reference.elf'
debug('run-01-prefix-after-reinit',elf,IDENTITY+'''p load_stage
p/x load_cid
p load_blocks
p load_clock_hz
p/x load_clkcr
p load_bus_card
p/x GPIOB->MODER
if load_stage != 2 || load_clock_hz != 10000000 || load_bus_card != 0 || load_producer_enabled != 0 || (GPIOB->MODER & 0xc00003c0) != 0
 quit 4
end
set $ok=transfer(0,rx,2048,65536)
p $ok
if $ok != 1
 quit 7
end
'''+ 'dump binary memory '+win(P/'run-01-prefix-after-reinit.bin')+''' rx rx+65536
p/x hsd2.ErrorCode
p/x load_last_r1
x/2wx 0xE000ED28
''')
b=(P/'run-01-prefix-after-reinit.bin').read_bytes();expected=bytes((x*37+(x>>8)+(x>>16)+(x>>24)*113)&255 for x in range(65536));assert b==expected
(P/'prefix-after-reinit-verification.json').write_text(json.dumps({'bytes':65536,'mismatches':0,'sha256':hashlib.sha256(b).hexdigest(),'boundary':'read after later hint firmware flash/software reset/card INIT; no physical power-loss; no change to LBA2048..2175','same_as_initial_prefix':b==(P/'run-01-prefix-0000.bin').read_bytes()},indent=2)+'\n')
print('prefix retained after controlled reinit: 65536 bytes exact; second board safe, stage2, no auto-write')
