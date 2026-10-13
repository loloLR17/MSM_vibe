from pathlib import Path
import importlib.util,hashlib,json,struct
p=Path('/tmp/TR2-mission015-U');p.mkdir(exist_ok=True)
spec=importlib.util.spec_from_file_location('h','/home/lolo/dev/msm/reports/TR2-mission015-J-20261012T184145Z/hardware.py');h=importlib.util.module_from_spec(spec);spec.loader.exec_module(h);h.P=p
old=Path('/home/lolo/dev/msm/reports/TR2-mission014-H-20261012T073729Z/build-production-live');elf=old/'tr2_stm32_p11c.elf';binary=old/'tr2_stm32_p11c.bin'
assert hashlib.sha256(binary.read_bytes()).hexdigest()=='b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611'
assert hashlib.sha256(elf.read_bytes()).hexdigest()=='c37ad8370fbfe3da35c0e5c16d7239d18e7d0ad102f33db79a67a029ecfca8de'
prefix=h.win(p).replace('\\','/')
h.debug('initial-preserve',elf,'''x/3wx 0x0BFA0700
p tr2_production_stage
p tr2_production_result
p production_runtime.recovery
p production_runtime.application.runtime.configuration_service.recovery_status
x/3wx 0xE000E100
x/2wx 0xE000ED28
'''+f'dump binary memory {prefix}/flash-before.bin 0x08000000 0x08200000\ndump binary memory {prefix}/flash-before-second.bin 0x08000000 0x08200000\ndump binary memory {prefix}/ram-before.bin 0x20000000 0x200C0000\ndump binary memory {prefix}/uid.bin 0x0BFA0700 0x0BFA070C\n')
uid=struct.unpack('<3I',(p/'uid.bin').read_bytes());assert uid==(0x004A003D,0x38425018,0x20333157)
assert (p/'flash-before.bin').read_bytes()==(p/'flash-before-second.bin').read_bytes()
assert (p/'flash-before.bin').read_bytes()[:len(binary.read_bytes())]==binary.read_bytes()
(p/'backup-verified.json').write_text(json.dumps({'uid':[f'{v:08X}' for v in uid],'flash_sha256':hashlib.sha256((p/'flash-before.bin').read_bytes()).hexdigest(),'double_flash_identical':True,'M014_H_prefix_exact':True,'bin_sha256':hashlib.sha256(binary.read_bytes()).hexdigest()},indent=2)+'\n')
