from pathlib import Path
import hardware_primary_readonly as h
P=Path(__file__).parent
elf=Path('/home/lolo/dev/msm/reports/TR2-accel-fs8g-ac/firmware/tr2-live-operator-NON-PRODUCTION.elf')
h.debug('TR2-001-final-RAM-state-only',elf,h.IDENTITY+'''p r4u_result
p r4r_result.calibration_residual_us
p work.error
p owner.acquisition.fifo.running
p diag_fifo.running
p configuration.active.config_id
p configuration.active.generation
p configuration.active.payload.full_scale_code
p engine.last
p/x *(unsigned int*)0x42020814
if r4u_result.stage!=7 || r4u_result.error || work.error || owner.acquisition.fifo.running || diag_fifo.running || configuration.active.payload.full_scale_code!=0
 quit 8
end
''')
print((P/'hardware/TR2-001-final-RAM-state-only.log').read_text())
