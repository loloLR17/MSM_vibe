from pathlib import Path
import sys,hashlib,json
sys.path.insert(0,'/tmp');import a0402_hardware as h
old=Path('/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z/build-cooperative4/tr2_stm32_p11c.elf')
assert hashlib.sha256(old.read_bytes()).hexdigest()=='0eb4e3b00a305905a0ac5002b5ef9041ae1ddb56911fba9b5c122c0c5b885f6a'
prefix=h.win(h.P).replace('\\','/')
body=h.IDENTITY+'''p SystemCoreClock
p tr2_production_stage
p tr2_production_result
p production_runtime.recovery
p cooperative.state
p cooperative.cleanup_result
p cooperative.cleanup
p tr2_fifo_state
x/3wx 0xE000E100
x/2wx 0xE000ED28
if tr2_production_stage != 2 || tr2_production_result != 0 || production_runtime.recovery.generation != 12 || (cooperative.state != 4 && cooperative.state != 5)
 quit 4
end
'''+f'dump binary memory {prefix}/flash-before.bin 0x08000000 0x08200000\ndump binary memory {prefix}/persistent-before.bin production_runtime.candidate (production_runtime.candidate+51914)\n'
h.debug('initial-mcu-preflight',old,body)
repo=Path('/home/lolo/dev/msm/projects/MSM_vibe-A04-02')
golden=bytes.fromhex((repo/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261013-M015-V/qualified-cumulative4.bin.hex').read_text());flash=(h.P/'flash-before.bin').read_bytes();assert len(flash)==2097152 and flash[:len(golden)]==golden
(h.P/'initial-preservation.json').write_text(json.dumps(dict(old_elf_sha256=hashlib.sha256(old.read_bytes()).hexdigest(),golden_prefix_exact=True,flash_bytes=len(flash),flash_sha256=hashlib.sha256(flash).hexdigest(),persistent_bytes=(h.P/'persistent-before.bin').stat().st_size,persistent_sha256=hashlib.sha256((h.P/'persistent-before.bin').read_bytes()).hexdigest(),uid='004A003D3842501820333157',sn=h.SN,backup_single_read_with_golden_verification=True),indent=2)+'\n')
print('Current Flash exact golden prefix; full backup and generation12 preserved.')
