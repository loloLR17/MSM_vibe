import hardware as h
from pathlib import Path
import json,time,datetime
elf=h.P/'qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf'
attempt=h.P/'attempt02-run-attempt.json'
assert not attempt.exists(),'Never retry a physical run automatically'
attempt.write_text(json.dumps({'utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'one_ram_trigger':'0x4D313657','region_base':16777216,'region_bytes':8388608,'code_sha':'38c027364a1d51465ae26678deacec7369df3f3d','bin_sha256':'00ba4f5b6750f3c20b2bd18832c99eaf10ac257449df6df12a53f6500ed81e5d','retry':0},indent=2)+'\n')
h.debug('attempt02-single-trigger',elf,h.IDENTITY+"""if storage_qual_stage != 2 || storage_qual_result != 0 || storage_qual_scan_nonzero != 0 || storage_qual_recovery_status[0] != 1 || storage_qual_recovery_status[1] != 1
 quit 4
end
set variable storage_qual_trigger=0x4D313657
""")
time.sleep(30)
h.debug('attempt02-run-results',elf,h.IDENTITY+"""p storage_qual_stage
p storage_qual_result
p storage_qual_samples
p storage_qual_checkpoints
p storage_qual_append_ms
p storage_qual_max_call_ms
p storage_qual_capacity_mb
p storage_qual_configuration_available
p storage_qual_configuration_generation
p storage_qual_recovery_status
p storage_qual_recovery_bytes
p config_service.has_active
p config_service.active
p fram_media.physical.write
p SystemCoreClock
x/2wx 0xE000ED28
""")
print((h.P/'attempt02-run-results.log').read_text())
