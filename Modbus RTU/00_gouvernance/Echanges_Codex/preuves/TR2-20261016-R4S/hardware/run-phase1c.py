from hardware import *
local=P.parent;root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4S-service-budget-safe-recovery')
pre=json.loads((P/'preflight-summary.json').read_text());assert pre['match_R4R_final']
old=local/'probe-build-02/tr2-r4s-readonly-service-NON-PRODUCTION.elf'
debug('phase1c-before-flash',old,IDENTITY+'p owner.acquisition.fifo.stopped\nif owner.acquisition.fifo.running || !owner.acquisition.fifo.stopped\n quit 7\nend\n')
image=local/'probe-build-03/tr2-r4s-readonly-service-NON-PRODUCTION.bin';elf=image.with_suffix('.elf')
inv=(local/'stlink-inventory.log').read_text();assert inv.count('ST-LINK SN')==1 and SN in inv
r=dict(code_sha=subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip(),bin_bytes=image.stat().st_size,bin_sha256=hashlib.sha256(image.read_bytes()).hexdigest(),elf_sha256=hashlib.sha256(elf.read_bytes()).hexdigest(),profile=(image.parent/'build-profile.txt').read_text())
(local/'phase1c-build.json').write_text(json.dumps(r,indent=2)+'\n')
flash('phase1c-flash',image,r['code_sha'])
debug('phase1c-reset-readback',elf,IDENTITY+f'dump binary memory {win(P/"phase1c-readback.bin")} 0x08000000 {hex(0x08000000+image.stat().st_size)}\nmonitor reset\n')
assert (P/'phase1c-readback.bin').read_bytes()==image.read_bytes();time.sleep(2)
debug('phase1c-trigger12-result',elf,IDENTITY+'''p r4s_result
if r4s_result.stage != 1 || r4s_result.error != 0
 quit 4
end
break r4r_checkpoint
set r4_trigger=0x5234000C
continue
p r4s_result
p r4r_result
p r4_result
p work
p r4s_max_service
p r4s_trace
p event_time
p metric
p r4r_first_value
p engine.indeterminate
p store.effect
p owner.acquisition.fifo.stopped
printf "FINAL_STAGE=%u ERROR=%u CAL_ERROR=%u\\n",r4s_result.stage,r4s_result.error,r4r_result.error
printf "FIRST_RMS=%u,%u,%u,%u PEAK=%u,%u,%u,%u N=%u DURATION=%u\\n",r4r_first_value.rms_global_mg,r4r_first_value.rms_x_mg,r4r_first_value.rms_y_mg,r4r_first_value.rms_z_mg,r4r_first_value.peak_global_mg,r4r_first_value.peak_x_mg,r4r_first_value.peak_y_mg,r4r_first_value.peak_z_mg,r4r_first_value.valid_sample_count,r4r_first_value.window_duration_ms
'''+f'''dump binary memory {win(P/'phase1c-raw.bin')} r4r_raw r4r_raw+7*r4r_result.raw_words
dump binary memory {win(P/'phase1c-brackets.bin')} r4r_brackets &r4r_brackets[r4r_result.brackets]
dump binary memory {win(P/'phase1c-xyz.bin')} r4r_xyz (char*)r4r_xyz+sizeof(r4r_xyz)
dump binary memory {win(P/'phase1c-trace.bin')} r4s_trace &r4s_trace[r4s_result.trace_count]
if r4s_result.clock_checks > 0
dump binary memory {win(P/'phase1c-clock-checks.bin')} r4s_clock_brackets &r4s_clock_brackets[r4s_result.clock_checks]
end
set $r=(int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r
if $r != 0
 quit 6
end
dump binary memory {win(P/'fram-after-phase1c.bin')} baseline baseline+262144
''',timeout=100)
assert (P/'fram-after-phase1c.bin').read_bytes()==(P/'fram-before.bin').read_bytes()
print('Phase1 returned; full FRAM exact. Inspect result before phase2',flush=True)
