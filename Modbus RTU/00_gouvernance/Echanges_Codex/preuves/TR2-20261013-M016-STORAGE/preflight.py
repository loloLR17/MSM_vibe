from pathlib import Path
import hardware as h
import hashlib,json
old=Path('/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/build-artifacts')
elf=old/'tr2-cumulative-m014-m015-spi4.elf'
binary=old/'tr2-cumulative-m014-m015-spi4.bin'
assert hashlib.sha256(binary.read_bytes()).hexdigest()=='b4301d6f3325c266edc19297413fd0885828e5625ec030a8acad340444b3fd9b'
assert hashlib.sha256(elf.read_bytes()).hexdigest()=='3a1310a77ba379bbc513dd57d0fbfdb7cab89aac50ab2e67400abddc903f40a1'
h.debug('current-cumulative-preflight',elf,h.IDENTITY+"""p tr2_production_stage
p tr2_production_result
p cooperative.state
p cooperative.cleanup
p cooperative_fram_state
p g_serial.tx_busy
p g_serial.tx_fault
p production_runtime.recovery
if tr2_production_stage != 2 || tr2_production_result != 0 || g_serial.tx_busy || g_serial.tx_fault
 quit 4
end
"""+'dump binary memory '+h.win(h.P/'current-cumulative.bin')+' 0x08000000 0x080297DC\n')
assert (h.P/'current-cumulative.bin').read_bytes()==binary.read_bytes()
print('Current Flash prefix exact A04-02; backup recovery image verified')
