from pathlib import Path
import sys,subprocess,json,hashlib
p=Path('/tmp/TR2-mission015-U');
import importlib.util
spec=importlib.util.spec_from_file_location('h','/home/lolo/dev/msm/reports/TR2-mission015-J-20261012T184145Z/hardware.py');h=importlib.util.module_from_spec(spec);spec.loader.exec_module(h);h.P=p
root='tr2-hp:C:/Users/Lolo/AppData/Local/TR2/M014-H-20261012T073729Z/physical-m015-u-restored/'
for name in ['physical.jsonl','result.json','gate.json']:
 dest=p/('hp-restored-'+name);res=subprocess.run(['/mnt/c/Windows/System32/OpenSSH/scp.exe','-o','BatchMode=yes','-o','StrictHostKeyChecking=yes',root+name,h.win(dest)],capture_output=True,timeout=30);(p/('collect-'+name+'.log')).write_bytes(res.stdout+res.stderr);res.check_returncode()
result=json.loads((p/'hp-physical-restored/stdout.log').read_text(encoding='utf-8-sig'));assert result['exitCode']==0;actual=hashlib.sha256((p/'hp-restored-physical.jsonl').read_bytes()).hexdigest();assert actual.upper()==result['journalHash'];print('Full HP journal retrieved, matching remote hash:',actual)
old=Path('/home/lolo/dev/msm/reports/TR2-mission014-H-20261012T073729Z/build-production-live/tr2_stm32_p11c.elf');prefix=h.win(p).replace('\\','/')
h.debug('final-production-after-physical-no-reset',old,'''p uwTick
p tr2_production_stage
p tr2_production_result
p tr2_production_poll_errors
p production_runtime.recovery
p production_runtime.persistent_identity
p production_runtime.application.runtime.configuration_service.recovery_status
p production_runtime.application.runtime.b1_image
p g_serial.tx_busy
p g_serial.tx_fault
p g_serial.uart.ErrorCode
p g_serial.event_head
p g_serial.event_tail
x/3wx 0xE000E100
x/2wx 0xE000ED28
'''+f'dump binary memory {prefix}/ram-final-after-physical.bin 0x20000000 0x200C0000\n')
