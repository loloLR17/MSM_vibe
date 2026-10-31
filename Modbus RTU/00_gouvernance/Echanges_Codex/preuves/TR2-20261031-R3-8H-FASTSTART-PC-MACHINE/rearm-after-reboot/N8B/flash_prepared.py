from pathlib import Path
import subprocess,json,hashlib
import hardware_second as h
P=Path(__file__).parent;R=Path('/home/lolo/dev/msm/projects/MSM_vibe-R3-8h-faststart')
code=subprocess.check_output(['git','rev-parse','HEAD'],cwd=R,text=True).strip();assert code=='75ea886cdd3ad8d37db3f5573b2e2f07299714e9'
assert not subprocess.check_output(['git','status','--porcelain'],cwd=R)
assert json.loads((P/'PREFLIGHT.json').read_text())['old_N8A_entire_BIN_identical']
image=P/'arm/r3_coex.bin';data=image.read_bytes();assert code.encode() in data and b'N8B0000.TR2' in data
assert '[100%] Built target r3_coex' in (P/'build.log').read_text()
meta=dict(code_sha=code,profile='v3/int16/8H exploratory',nominal_guard_us=30600000000,file_bytes=640*1024*1024,prefix='N8B',images=[dict(file=n,bytes=(P/'arm'/n).stat().st_size,sha256=hashlib.sha256((P/'arm'/n).read_bytes()).hexdigest()) for n in ['r3_coex.bin','r3_coex.elf','r3_coex.map']])
(P/'artifact.json').write_text(json.dumps(meta,indent=2)+'\n');h.flash('flash-N8B-prepared',image,code)
f=P/'hardware/N8B-readback.bin';assert not f.exists();path=h.win(f).replace('\\','/')
h.debug('N8B-readback-WAIT',P/'arm/r3_coex.elf',h.IDENTITY+f'''p r3_result
p r3_clock_fault
if r3_result.version!=4 || r3_result.stage!=2 || r3_result.error || r3_clock_fault || r3_command || fifo.running || r3_writer.active || witness_sequence || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end
dump binary memory {path} 0x08000000 0x{0x08000000+len(data):x}
''');assert f.read_bytes()==data
print('Prepared N8B image readback exact/WAIT/no SDinit; GDB detached',json.dumps(meta),flush=True)
