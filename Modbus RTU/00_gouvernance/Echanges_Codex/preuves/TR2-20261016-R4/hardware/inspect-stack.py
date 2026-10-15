from hardware import *
elf=Path('/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z/qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf')
debug('inspect-current-stack',elf,IDENTITY+'''bt
p tr2_commission_stage
p fram_media.physical
p/x &hspi1
p/x &tr2_commission_sd_chunk
''')
