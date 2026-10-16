from hardware import *
elf=P.parent/'bench-build/tr2-r4-qualification-NON-PRODUCTION.elf';image=elf.with_suffix('.bin')
root='/home/lolo/dev/msm/projects/MSM_vibe-R4-cooperative-fram';sha=subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip()
flash('bench5-program',image,sha)
debug('bench5-reset-and-flash-readback',elf,IDENTITY+'dump binary memory '+win(P/'bench5-flash-readback.bin')+' 0x08000000 '+hex(0x08000000+image.stat().st_size)+'\nmonitor reset\n')
assert (P/'bench5-flash-readback.bin').read_bytes()==image.read_bytes();time.sleep(2)
debug('bench5-recovery-trigger',elf,IDENTITY+'''p r4_result
if r4_result.stage != 1 || r4_result.error != 0
 quit 5
end
set var r4_trigger=0x52340003
''')
(P/'bench5-recovery-trigger-host-time.json').write_text(json.dumps(dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),not_before_observe_seconds=15))+'\n')
