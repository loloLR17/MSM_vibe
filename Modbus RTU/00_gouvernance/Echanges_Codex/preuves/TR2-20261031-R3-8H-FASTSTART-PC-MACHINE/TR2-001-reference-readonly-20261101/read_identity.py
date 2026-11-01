from pathlib import Path
import importlib.util,hashlib,json,subprocess,datetime
P=Path(__file__).parent;OLD=Path('/home/lolo/dev/msm/reports/TR2-accel-fs8g-ac');a=json.loads((OLD/'candidate-firmware.json').read_text());image=Path(a['bin_path']);elf=Path(a['elf_path'])
assert hashlib.sha256(image.read_bytes()).hexdigest()==a['bin_sha256'];assert hashlib.sha256(elf.read_bytes()).hexdigest()==a['elf_sha256']
spec=importlib.util.spec_from_file_location('hw','/home/lolo/dev/msm/reports/TR2-r3-N8E-duplex4-20261101/hardware_second.py');h=importlib.util.module_from_spec(spec);spec.loader.exec_module(h);h.P=P/'hardware';h.SN='003500463235510B37333439';h.IDENTITY='''x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
'''
(P/'STLINK_LIST_TR2_001.log').write_bytes(subprocess.check_output([h.CLI,'-l','stlink']))
f=P/'hardware/TR2-001-flash-prefix.bin'
body=h.IDENTITY+f'dump binary memory {h.win(f).replace(chr(92),"/")} 0x08000000 {hex(0x08000000+a["bin_bytes"])}\n'+'''p/x $pc
bt 5
p r4u_result
p engine.last
p configuration.active
p work.depth
p work.servicing
p runtime.main_busy
p runtime.servicing
p owner.acquisition.fifo.running
p owner.acquisition.fifo.error
p owner.acquisition.fifo.stop_error
p/x *(unsigned int*)0x46002008
p/x *(unsigned int*)0x4600200c
'''
h.debug('TR2-001-identity-image-and-RAM-readonly',elf,body,timeout=45)
b=f.read_bytes();same=b==image.read_bytes();out=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),UID='004A003D/38425018/20333157',SN=h.SN,bytes_read=len(b),sha256=hashlib.sha256(b).hexdigest(),matches_last_AC1_image=same,expected_code_sha=a['code_sha'],expected_bin_sha=a['bin_sha256'],target_function_called=False,flash=False,reset=False,START_STOP=False,FRAM_access=False,GDB_detached=True,sensor_connections_not_yet_confirmed=True)
(P/'IDENTITY_AND_IMAGE.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out));assert same,'Installed firmware differs: stop before interpreting ELF symbols'
