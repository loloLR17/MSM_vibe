from hardware import *
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4Q-live-b3-rtu');local=P.parent
pre=json.loads((P/'preflight-summary.json').read_text());assert pre['match_R4P_final'] and not pre['flash_or_persistent_write']
assert (P/'flash-before.bin').read_bytes()==Path('/home/lolo/dev/msm/reports/TR2-r4p/hardware/flash-before.bin').read_bytes()
assert (P/'fram-before.bin').read_bytes()==Path('/home/lolo/dev/msm/reports/TR2-r4p/hardware/fram-after-restoration.bin').read_bytes()
image=local/'probe-build/tr2-r4q-readonly-rtu-NON-PRODUCTION.bin';elf=image.with_suffix('.elf');record=json.loads((local/'build-artifacts.json').read_text())['probe']
assert hashlib.sha256(image.read_bytes()).hexdigest()==record['bin_sha256'] and hashlib.sha256(elf.read_bytes()).hexdigest()==record['elf_sha256']
assert (local/'stlink-inventory.log').read_text().count('ST-LINK SN')==1 and SN in (local/'stlink-inventory.log').read_text()
flash('r4q-flash-probe',image,record['code_sha'])
debug('r4q-reset-readback',elf,IDENTITY+f'dump binary memory {win(P/"probe-readback.bin")} 0x08000000 {hex(0x08000000+image.stat().st_size)}\nmonitor reset\n')
assert (P/'probe-readback.bin').read_bytes()==image.read_bytes();time.sleep(2)
debug('r4q-trigger9',elf,IDENTITY+'''p r4q_result
p r4_result
if r4q_result.stage != 1 || r4q_result.error != 0
 quit 4
end
set r4_trigger=0x52340009
''')
(P/'started.json').write_text(json.dumps(dict(epoch=time.time(),utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),code=record['code_sha'],profile='readonly FRAM, sensor stopped, RTU RAM loopback, no UART/RS485'),indent=2)+'\n')
