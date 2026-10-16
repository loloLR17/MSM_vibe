from hardware import *
B=Path('/home/lolo/dev/msm/reports/TR2-r4p/resume-build');image=B/'tr2-r4p-coexistence-NON-PRODUCTION.bin';elf=image.with_suffix('.elf')
assert hashlib.sha256((P/'fram-after-physical.bin').read_bytes()).hexdigest()=='713a953384f2688f8448989ba706bf0dcef33ace3f177a1ee90fa5eb63713ba0'
flash('r4p-corrected-flash',image,'9f429e5')
back=P/'r4p-corrected-image-readback.bin'
debug('r4p-corrected-image-reset',elf,IDENTITY+f'dump binary memory {win(back)} 0x08000000 0x{0x08000000+image.stat().st_size:08x}\nmonitor reset\n')
assert back.read_bytes()==image.read_bytes();time.sleep(2)
debug('r4p-readonly-trigger7',elf,IDENTITY+'''p r4_result
p r4m_guard
if r4_result.stage != 1 || r4_result.error != 0 || r4m_guard.phase != 0
 quit 4
end
set r4_trigger=0x52340007
''')
(P/'readonly-started.json').write_text(json.dumps(dict(epoch=time.time(),utc=datetime.datetime.now(datetime.timezone.utc).isoformat()),indent=2)+'\n')
