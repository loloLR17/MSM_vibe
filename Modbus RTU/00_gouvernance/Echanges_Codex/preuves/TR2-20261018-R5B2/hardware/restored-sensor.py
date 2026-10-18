from hardware import *
elf=Path('/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z/qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf')
body=(P/'sensor-stopped-readonly.gdb').read_text().split('target extended-remote localhost:61234\n',1)[1].split('\ndetach',1)[0]
debug('restored-sensor-stopped-readonly',elf,body,40)
