from hardware import *
elf=Path('/home/lolo/dev/msm/reports/TR2-r4m/bench-build/tr2-r4m-qualification-NON-PRODUCTION.elf')
# Run only after new image flash/readback and WAIT verified by resume-physical.py.
debug('r4m-bridge-second-read',elf,IDENTITY+'''p owner.acquisition.fifo.running
if owner.acquisition.fifo.running
 quit 4
end
set $r=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r
if $r != 0
 quit 5
end
'''+f'dump binary memory {win(P/"fram-bridge-second-read.bin")} baseline baseline+262144\n',timeout=60)
a=(P/'fram-bridge-second-read.bin').read_bytes();assert a==(P/'fram-after-physical.bin').read_bytes()
print('BRIDGE DOUBLE READ',hashlib.sha256(a).hexdigest())
