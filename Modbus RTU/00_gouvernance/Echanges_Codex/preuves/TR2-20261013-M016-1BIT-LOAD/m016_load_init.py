from pathlib import Path
import sys,time,subprocess
P=Path(Path('/tmp/m016-load-proof').read_text());sys.path.insert(0,str(P));from hardware import *
elf=Path('/home/lolo/dev/msm/reports/M016-1bit-load-build/sdmmc_reference.elf')
z=subprocess.run([GDB,'-nx','-q','-batch','-ex','info line load_main.c:207',win(elf)],capture_output=True);(P/'terminal-line.log').write_bytes(z.stdout+z.stderr);z.check_returncode();print(z.stdout.decode(errors='replace'))
debug('load-initialize-once',elf,IDENTITY+'''p load_stage
p load_trigger
if load_stage != 1 || load_trigger != 0
 quit 4
end
set load_trigger=0x4C31494E
''');time.sleep(3)
debug('load-preflight-card',elf,IDENTITY+'''p load_stage
p load_error
p/x load_cid
p load_blocks
p load_kernel_hz
p load_clock_hz
p/x load_clkcr
p load_bus_card
p/x hsd2.ErrorCode
p/x load_last_r1
p/x GPIOB->MODER
p SystemCoreClock
p load_generated
p load_written
if load_stage != 2 || load_error != 0 || load_clock_hz != 10000000 || load_bus_card != 0 || load_generated != 0 || load_written != 0
 quit 5
end
''');print((P/'load-preflight-card.log').read_text(errors='replace'))
