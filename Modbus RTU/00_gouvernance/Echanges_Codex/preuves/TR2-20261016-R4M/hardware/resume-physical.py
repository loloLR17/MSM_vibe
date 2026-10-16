from hardware import *
import runpy,shutil
B=Path('/home/lolo/dev/msm/reports/TR2-r4m/bench-build')
image=B/'tr2-r4m-qualification-NON-PRODUCTION.bin';elf=image.with_suffix('.elf')
assert hashlib.sha256((P/'fram-after-physical.bin').read_bytes()).hexdigest()=='c868950fe3c60f9a3b23a9ce8d565029ee57d4f06b08cbf6062de9238b2af950'
shutil.copy2(image,P/'r4m-resume-image.bin');shutil.copy2(elf,P/'r4m-resume-image.elf')
flash('r4m-resume-flash',image,'3606942')
back=P/'r4m-resume-image-readback.bin'
debug('r4m-resume-image-reset',elf,IDENTITY+f'dump binary memory {win(back)} 0x08000000 0x{0x08000000+image.stat().st_size:08x}\nmonitor reset\n')
assert back.read_bytes()==image.read_bytes()
time.sleep(2)
runpy.run_path(str(P/'backup-bridge.py'))
debug('r4m-resume-trigger6',elf,IDENTITY+'''p r4_result
p r4m_guard
if r4_result.stage != 1 || r4_result.error != 0 || r4m_guard.phase != 0
 quit 4
end
set r4m_backup_token=0x52344D42
set r4_trigger=0x52340006
''')
(P/'r4m-resume-started.json').write_text(json.dumps(dict(epoch=time.time(),utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),code_sha='3606942',image_sha=hashlib.sha256(image.read_bytes()).hexdigest(),image_bytes=image.stat().st_size,no_second_isolation=True),indent=2)+'\n')
