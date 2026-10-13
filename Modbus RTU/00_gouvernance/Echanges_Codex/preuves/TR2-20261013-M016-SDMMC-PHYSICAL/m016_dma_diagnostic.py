from pathlib import Path
import sys,json
P=Path(Path('/tmp/m016-sd-physical-proof').read_text());sys.path.insert(0,str(P));from hardware import *
b=Path('/home/lolo/dev/msm/reports/M016-sdmmc-reference-armed-build')
debug('attempt01-dma-layout',b/'sdmmc_reference.elf',IDENTITY+'''p/x &node
p/x &list
p sizeof(node)
p/x node
p/x list
p/x hsd2.Instance->IDMABAR
p/x hsd2.Instance->IDMALAR
p/x hsd2.Instance->IDMABASER
p/x hsd2.Instance->IDMABSIZE
p/x hsd2.Instance->STA
p/x hsd2.Instance->DCTRL
''')
rx=(P/'attempt01-rx.bin').read_bytes();expected=bytes((i*37+(i>>8))&255 for i in range(4096));(P/'attempt01-analysis.json').write_text(json.dumps({'status':'FAILED','phase':'read','rounds_passed':0,'hal_error':'0x1002 DATA_CRC_FAIL | RX_OVERRUN','write_ms':991,'rx_matches_first_pattern_bytes':sum(a==b for a,b in zip(rx,expected)),'rx_sha256':hashlib.sha256(rx).hexdigest(),'no_compare_success_claim':True},indent=2)+'\n');print((P/'attempt01-dma-layout.log').read_text(errors='replace'))
