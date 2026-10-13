import hardware as h
from pathlib import Path
import json,time,datetime
elf=h.P/'qualification-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf'
attempt=h.P/'qualification-run-attempt.json'
assert not attempt.exists(),'Never retry a physical run automatically'
attempt.write_text(json.dumps({'utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'one_ram_trigger':'0x4D313657','region_base':8388608,'region_bytes':16777216,'code_sha':'9e8bf347b90c0747f310c37a4430982a33966bce','bin_sha256':'d1b736a8027fbd373c4ce36c15a0a9f4d7a2d882a92e876e1ea95a24fb09b463','retry':0},indent=2)+'\n')
h.debug('qualification-single-trigger',elf,h.IDENTITY+"""if storage_qual_stage != 2 || storage_qual_result != 0 || storage_qual_scan_nonzero != 0 || storage_qual_recovery_status[0] != 1 || storage_qual_recovery_status[1] != 1
 quit 4
end
set variable storage_qual_trigger=0x4D313657
""")
time.sleep(30)
h.debug('qualification-run-results',elf,h.IDENTITY+"""p storage_qual_stage
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
print((h.P/'qualification-run-results.log').read_text())
