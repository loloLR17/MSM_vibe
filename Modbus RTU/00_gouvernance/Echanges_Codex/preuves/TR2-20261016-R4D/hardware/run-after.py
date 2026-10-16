from hardware import *
import subprocess
B=Path('/home/lolo/dev/msm/reports/TR2-r4d/after-build');image=B/'tr2-r4d-diagnostic-NON-PRODUCTION.bin';elf=image.with_suffix('.elf')
symbols=subprocess.check_output(['arm-none-eabi-nm',str(elf)],text=True)
for forbidden in ['live_command_engine_submit','live_command_store_commit','r4m_isolate_journal','stm32_fram_storage_write']:assert forbidden not in symbols,forbidden
assert hashlib.sha256((P/'fram-before.bin').read_bytes()).hexdigest()=='f2bd7a99d3d0e9f17292310f2cec51fe3a3f521ba3a4d2f07da5b6302258a0c1'
flash('r4d-after-flash',image,'7a6a8b924f4e616adfcb5fb2dc52c7ccc16297e0')
back=P/'r4d-after-image-readback.bin'
debug('r4d-after-readback-reset',elf,IDENTITY+f'dump binary memory {win(back)} 0x08000000 0x{0x08000000+image.stat().st_size:08x}\nmonitor reset\n')
assert back.read_bytes()==image.read_bytes();time.sleep(2)
debug('r4d-after-trigger',elf,IDENTITY+'''p r4d
if r4d.stage != 1 || r4d.error != 0
 quit 4
end
set r4d_trigger=0x52344402
''')
(P/'after-started.json').write_text(json.dumps(dict(epoch=time.time(),utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),expected_wait=15),indent=2)+'\n')
