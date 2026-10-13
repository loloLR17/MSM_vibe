from pathlib import Path
import sys
P=Path(Path('/tmp/m016-sd-physical-proof').read_text());sys.path.insert(0,str(P));from hardware import *
debug('second-identity-before-flash',Path('/home/lolo/dev/msm/reports/M016-sdmmc-reference-build/sdmmc_reference.elf'),IDENTITY+'x/8wx 0x08000000\nx/2wx 0xE000ED28\n')
print((P/'second-identity-before-flash.log').read_text(errors='replace'))
