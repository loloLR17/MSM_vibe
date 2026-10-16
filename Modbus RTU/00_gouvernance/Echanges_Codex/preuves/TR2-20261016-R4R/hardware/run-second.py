from hardware import *
local=P.parent;root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4R-physical-time-b3')
old=local/'probe-build/tr2-r4r-physical-time-NON-PRODUCTION.elf'
debug('r4r-before-second-flash',old,IDENTITY+'''p r4r_result
p owner.acquisition.fifo.stopped
if owner.acquisition.fifo.running || !owner.acquisition.fifo.stopped
 quit 4
end
''')
image=local/'probe-build-02/tr2-r4r-physical-time-NON-PRODUCTION.bin';elf=image.with_suffix('.elf')
record=dict(code_sha=subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip(),bin_bytes=image.stat().st_size,bin_sha256=hashlib.sha256(image.read_bytes()).hexdigest(),elf_sha256=hashlib.sha256(elf.read_bytes()).hexdigest(),profile=(image.parent/'build-profile.txt').read_text())
(local/'event-build.json').write_text(json.dumps(record,indent=2)+'\n')
flash('r4r-flash-event-probe',image,record['code_sha'])
debug('r4r-event-reset-readback',elf,IDENTITY+f'dump binary memory {win(P/"event-probe-readback.bin")} 0x08000000 {hex(0x08000000+image.stat().st_size)}\nmonitor reset\n')
assert (P/'event-probe-readback.bin').read_bytes()==image.read_bytes();time.sleep(2)
debug('r4r-trigger10-complete',elf,IDENTITY+'''p r4r_result
if r4r_result.stage != 1 || r4r_result.error != 0
 quit 5
end
break r4r_checkpoint
set r4_trigger=0x5234000A
continue
p r4r_result
p diag_fifo
p r4_result
p work
printf "CHECKPOINT_STAGE=%u ERROR=%u\\n",r4r_result.stage,r4r_result.error
'''+f'''dump binary memory {win(P/'raw02.bin')} r4r_raw r4r_raw+7*r4r_result.raw_words
dump binary memory {win(P/'brackets02.bin')} r4r_brackets &r4r_brackets[r4r_result.brackets]
set $read=(int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $read
if $read != 0
 quit 7
end
dump binary memory {win(P/'fram-after-calibration.bin')} baseline baseline+262144
''',timeout=90)
assert b'CHECKPOINT_STAGE=11 ERROR=0' in (P/'r4r-trigger10-complete.log').read_bytes()
assert (P/'fram-after-calibration.bin').read_bytes()==(P/'fram-before.bin').read_bytes()
print('CALIBRATION COMPLETE stage11, full FRAM unchanged; target WAIT safe for analysis',flush=True)
