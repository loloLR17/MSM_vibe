from pathlib import Path
import sys
sys.path.insert(0,'/tmp');import a0402_hardware as h;import a0402_hp as hp
elf=h.P/'build-artifacts/tr2-cumulative-m014-m015-spi4.elf'
h.debug('before-baseline-gate',elf,h.IDENTITY+'''p cooperative.state
p production_runtime.recovery
if tr2_production_stage != 2 || tr2_production_result != 0 || cooperative.state != 1 || bench_ready != 1 || cooperative_arm || cooperative_start_tick || g_serial.tx_busy || g_serial.tx_fault || g_serial.uart.ErrorCode
 quit 4
end
''')
hp.invoke('Baseline',hp.gate())
