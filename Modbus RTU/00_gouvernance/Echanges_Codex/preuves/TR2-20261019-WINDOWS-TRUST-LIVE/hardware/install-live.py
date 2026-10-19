from hardware import *
f=Path('/home/lolo/dev/msm/reports/TR2-uxl2-physical/firmware')
b=f/'tr2-live-operator-NON-PRODUCTION.bin'; e=f/'tr2-live-operator-NON-PRODUCTION.elf'
assert hashlib.sha256(b.read_bytes()).hexdigest()=='9bbe397911185b798a0b030aba893a325bfdf6afa6947145bda9157f3bff0981'
assert hashlib.sha256(e.read_bytes()).hexdigest()=='ba999ab4a8aa674a977ca8e3ab95469eba46c0d12ff2cdbf98c89bcfb1a1cbe3'
flash('live-install',b,'c36c971')
time.sleep(15)
debug('live-boot',e,IDENTITY+'''p r4u_result
p owner.acquisition.fifo
p engine
p store
p live
'''+f'dump binary memory {win(P/"live-readback.bin")} 0x08000000 {hex(0x08000000+b.stat().st_size)}\n')
assert (P/'live-readback.bin').read_bytes()==b.read_bytes()
print('LIVE_IMAGE_READBACK_EXACT',flush=True)
