from hardware import *
B=Path('/home/lolo/dev/msm/reports/TR2-r4m/bench-build')
image=B/'tr2-r4m-qualification-NON-PRODUCTION.bin';elf=image.with_suffix('.elf')
for kind,size,sha in [('flash',2097152,'c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26'),('fram',262144,'c814e9cc32eff06016f28ef964233354e35ba458653a16434e4416b296271561')]:
 a=(P/(kind+'-before.bin')).read_bytes();b=(P/(kind+'-second-read.bin')).read_bytes()
 assert a==b and len(a)==size and hashlib.sha256(a).hexdigest()==sha
flash('r4m-flash',image,'bc3e83de050a527cc839b494025c906f5c06246b')
back=P/'r4m-image-readback.bin'
debug('r4m-image-readback-reset',elf,IDENTITY+f'dump binary memory {win(back)} 0x08000000 0x{0x08000000+image.stat().st_size:08x}\nmonitor reset\n')
assert back.read_bytes()==image.read_bytes()
time.sleep(2)
debug('r4m-trigger4',elf,IDENTITY+'''p r4_result
p r4m_guard
if r4_result.stage != 1 || r4_result.error != 0 || r4m_guard.phase != 0
 quit 4
end
set r4m_backup_token = 0x52344D42
set r4_trigger = 0x52340004
''')
(P/'r4m-started.json').write_text(json.dumps(dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),epoch=time.time(),code_sha='bc3e83de050a527cc839b494025c906f5c06246b',image_sha=hashlib.sha256(image.read_bytes()).hexdigest(),image_bytes=image.stat().st_size,wait_before_next_debug_seconds=300),indent=2)+'\n')
