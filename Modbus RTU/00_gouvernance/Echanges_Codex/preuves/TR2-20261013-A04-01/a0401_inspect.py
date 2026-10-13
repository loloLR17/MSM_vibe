from pathlib import Path
import subprocess,hashlib,json,shlex,re
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-A04-01');p=Path(Path('/tmp/tr2-a0401-proof').read_text());f=r/'Modbus RTU/05_Firmware';results=[];summary=[]
profiles=[('cumulative',f/'build-stm32-cumulative','tr2-cumulative-m014-m015-spi4'),('qualification-U',p/'build-qualification-U','tr2-special-st_fifo_window-NON-CUMULATIVE'),('legacy',p/'build-legacy','tr2-legacy-SD-WRITES-NON-PRODUCTION')]
for label,b,name in profiles:
 elf=b/(name+'.elf');binary=b/(name+'.bin')
 for suffix,cmd in [('symbols',['arm-none-eabi-nm','-n',str(elf)]),('sections',['arm-none-eabi-size','-A',str(elf)]),('main-disassembly',['arm-none-eabi-objdump','-d','--disassemble=main',str(elf)]),('compile-commands',['ninja','-C',str(b),'-t','commands'])]:
  z=subprocess.run(cmd,capture_output=True);assert z.returncode==0; (p/f'{label}-{suffix}.log').write_bytes(z.stdout);results.append(dict(command=cmd,return_code=z.returncode))
 symbols=(p/f'{label}-symbols.log').read_text();main=(p/f'{label}-main-disassembly.log').read_text()
 has_sd=bool(re.search(r'\bHAL_SD_WriteBlocks\b',symbols));assert has_sd==(label=='legacy'),label
 assert ('<Sdmmc2_Bringup>' in main)==(label=='legacy'),label
 if label=='cumulative':assert '<stm32_production_server_run>' in main
 if label=='qualification-U':assert '<stm32_iis3dwb_st_window_run>' in main
 (p/f'{label}-build-profile.txt').write_bytes((b/'build-profile.txt').read_bytes())
 (p/f'{label}-CMakeCache.txt').write_bytes((b/'CMakeCache.txt').read_bytes())
 for func in (['stm32_production_board_initialize','stm32_production_binding_acquire','stm32_production_server_run','storage_available','unavailable_action','unavailable_append'] if label=='cumulative' else ['stm32_iis3dwb_st_window_run'] if label=='qualification-U' else ['Sdmmc2_Bringup']):
  cmd=['arm-none-eabi-objdump','-d','--disassemble='+func,str(elf)];z=subprocess.run(cmd,capture_output=True);assert z.returncode==0;(p/f'{label}-{func}.log').write_bytes(z.stdout);results.append(dict(command=cmd,return_code=0))
 summary.append(dict(profile=label,bin_bytes=binary.stat().st_size,bin_sha256=hashlib.sha256(binary.read_bytes()).hexdigest(),elf_sha256=hashlib.sha256(elf.read_bytes()).hexdigest(),sd_writeblocks_linked=has_sd,sd_bringup_main_call=label=='legacy'))
# Exact bytes, not just a SHA comparison, with the immutable published golden.
golden=r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261013-M015-V/qualified-cumulative4.bin.hex'
b=profiles[0][1]/(profiles[0][2]+'.bin');g=bytes.fromhex(golden.read_text());assert b.read_bytes()==g
assert len(g)==169380 and hashlib.sha256(g).hexdigest()=='8529630a5a6c9aaa2449fde512b06d6ca16a89a08b7afea2abc9ecca3b75d480'
# Preprocessor guard from the real build command, using real headers/toolchain.
lines=(p/'cumulative-compile-commands.log').read_text().splitlines()
cmd=shlex.split(next(line for line in lines if '/main.c' in line and ' -c ' in line))
cmd=cmd[:cmd.index('-MD')]+['-E',str(f/'platform/stm32/main.c'),'-o','/dev/null']
cmd=[re.sub(r'(-DTR2_STM32_(?:PRODUCTION|MANUFACTURING|COMMISSIONING_READONLY|MODBUS_DIAGNOSTIC|ST_FIFO_WINDOW|CONTINUOUS_FIFO|CTRL1_DIFFERENTIAL|BURST_CAPTURE|ACTIVE_READBACK|REGISTER_PROBE|FIFO_CAPTURE|ACQUISITION_CAPTURE|FIFO_COOPERATIVE|IRQ_DIAGNOSTICS|RS485_TX_QUALIFIED|LEGACY_SD_HARNESS))=\d+',r'\1=0',x) for x in cmd]
for label,command,expected in [('direct-implicit',cmd,1),('direct-legacy',[x.replace('TR2_STM32_LEGACY_SD_HARNESS=0','TR2_STM32_LEGACY_SD_HARNESS=1') for x in cmd],0)]:
 z=subprocess.run(command,capture_output=True);(p/(label+'.log')).write_bytes(z.stderr);assert (z.returncode!=0)==bool(expected)
 if expected:assert b'Implicit generic SD harness forbidden' in z.stderr
 results.append(dict(case=label,command=command,return_code=z.returncode))
(p/'artifact-inspection.json').write_text(json.dumps(dict(profiles=summary,golden_byte_identical=True,golden_source=str(golden),commands=results),indent=2)+'\n')
print(json.dumps(summary,indent=2))
