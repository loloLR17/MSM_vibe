from pathlib import Path
import sys,time,subprocess,json,hashlib
P=Path('/home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014');sys.path.insert(0,str(P));from hardware import *
elf=Path('/home/lolo/dev/msm/reports/SDMMC1-PoC-release-build/sdmmc1_poc.elf')
debug('native-recovery-same-image',elf,IDENTITY+'p/x hsd1.ErrorCode\np/x hsd1.Instance->RESP1\nmonitor reset\n');time.sleep(8)
a=json.loads((P/'usb-volume-read-invocation.json').read_text());c=a['command'].replace('usb-volume-read-original-128KiB.bin','usb-volume-read-after-reset-128KiB.bin')
z=subprocess.run(['/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe','-NoProfile','-NonInteractive','-Command',c],capture_output=True,timeout=90);(P/'usb-volume-after-reset-result.json').write_bytes(z.stdout);(P/'usb-volume-after-reset-invocation.json').write_text(json.dumps({'command':c,'return_code':z.returncode,'explicit_recovery_attempt':1,'same_original_image_bus_and_clock':True},indent=2)+'\n');print(z.stdout.decode(errors='replace'))
if z.returncode==0:
 b=(P/'usb-volume-read-after-reset-128KiB.bin').read_bytes();e=b''.join((P/f'payload-{j:02d}.bin').read_bytes() for j in range(16));bad=[i for i,(x,y) in enumerate(zip(b,e)) if x!=y];r={'bytes':len(b),'mismatches':len(bad),'first_mismatch':bad[0] if bad else None,'sha256_read':hashlib.sha256(b).hexdigest(),'sha256_expected':hashlib.sha256(e).hexdigest(),'explicit_recovery_attempt':1};(P/'usb-integral-after-reset.json').write_text(json.dumps(r,indent=2)+'\n');print(r)
