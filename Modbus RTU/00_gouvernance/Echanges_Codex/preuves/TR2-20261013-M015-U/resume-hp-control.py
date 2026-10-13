from pathlib import Path
import subprocess,sys,base64,datetime,json,re
p=Path('/tmp/TR2-mission015-U');
import importlib.util
spec=importlib.util.spec_from_file_location('h','/home/lolo/dev/msm/reports/TR2-mission015-J-20261012T184145Z/hardware.py');h=importlib.util.module_from_spec(spec);spec.loader.exec_module(h);h.P=p
wrapper=p/'hp-tools';sys.path.insert(0,str(wrapper));import run_commissioning_h as hp
# Calibration read only, then fresh target gate without reset/flash.
body="[DateTimeOffset]::UtcNow.ToString('o')";encoded=base64.b64encode(body.encode('utf-16le')).decode();before=datetime.datetime.now(datetime.timezone.utc);read=subprocess.run([hp.SSH,'-T','-o','BatchMode=yes','-o','StrictHostKeyChecking=yes','-o','ConnectTimeout=10','tr2-hp','powershell.exe -NoProfile -NonInteractive -EncodedCommand '+encoded],capture_output=True,timeout=25);after=datetime.datetime.now(datetime.timezone.utc);(p/'hp-clock-read.log').write_bytes(read.stdout+read.stderr);read.check_returncode();clock=datetime.datetime.fromisoformat(read.stdout.decode().strip());offset=clock-after;(p/'clock-offset-restored.json').write_text(json.dumps({'host_before':before.isoformat(),'host_after':after.isoformat(),'hp_utc':clock.isoformat(),'offset_lower_bound_seconds':offset.total_seconds()},indent=2))
old=Path('/home/lolo/dev/msm/reports/TR2-mission014-H-20261012T073729Z/build-production-live/tr2_stm32_p11c.elf')
h.debug('restored-physical-baseline-no-reset',old,'''printf "GATE stage=%u result=%u errors=%u gen=%u uart=%u tx=%u tick=%u\\n", tr2_production_stage, tr2_production_result, tr2_production_poll_errors, production_runtime.recovery.generation, g_serial.uart.ErrorCode, g_serial.tx_busy, uwTick
p production_runtime.application.runtime.configuration_service.recovery_status
x/3wx 0xE000E100
x/2wx 0xE000ED28
''')
log=(p/'restored-physical-baseline-no-reset.log').read_text();assert re.search(r'GATE stage=2 result=0 errors=0 gen=12 uart=0 tx=0 tick=\d+',log);assert 'CONFIGURATION_RECOVERY_UNAVAILABLE' in log;assert '0x00020000\t0x00000004' in log;assert '0xe000ed28:\t0x00000000\t0x00000000' in log
host=datetime.datetime.now(datetime.timezone.utc);gate=host+offset;(p/'restored-physical-gate.json').write_text(json.dumps({'host_utc':host.isoformat(),'hp_clock_gate_utc':gate.isoformat(),'maximum_requests':2,'retry':0,'debuggers_closed':True,'reset':False},indent=2))
args=[sys.executable,str(wrapper/'run_commissioning_h.py'),'Steady','--metadata',str(p/'hp-package-restored.json'),'--evidence',str(p/'hp-physical-restored'),'--gate-utc',gate.isoformat()]
res=subprocess.run(args,capture_output=True,timeout=100);(p/'hp-physical-restored-invocation.log').write_bytes(res.stdout+res.stderr);print(res.stdout.decode());res.check_returncode()
