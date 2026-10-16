from hardware import *
local=P.parent;root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4R-physical-time-b3')
pre=json.loads((P/'preflight-summary.json').read_text());assert pre['match_R4P_final']
image=local/'probe-build/tr2-r4r-physical-time-NON-PRODUCTION.bin';elf=image.with_suffix('.elf')
record=json.loads((local/'diagnostic-build.json').read_text())
assert hashlib.sha256(image.read_bytes()).hexdigest()==record['bin_sha256']
assert hashlib.sha256(elf.read_bytes()).hexdigest()==record['elf_sha256']
inv=(local/'stlink-inventory.log').read_text();assert inv.count('ST-LINK SN')==1 and SN in inv
flash('r4r-flash-diagnostic',image,record['code_sha'])
debug('r4r-reset-readback',elf,IDENTITY+f'dump binary memory {win(P/"probe-readback.bin")} 0x08000000 {hex(0x08000000+image.stat().st_size)}\nmonitor reset\n')
assert (P/'probe-readback.bin').read_bytes()==image.read_bytes();time.sleep(2)
debug('r4r-trigger10',elf,IDENTITY+'''p r4r_result
p r4_result
if r4r_result.stage != 1 || r4r_result.error != 0
 quit 4
end
set r4_trigger=0x5234000A
''')
print('TRIGGERED one bounded 2s sensor/time diagnostic; no NVM command',flush=True)
