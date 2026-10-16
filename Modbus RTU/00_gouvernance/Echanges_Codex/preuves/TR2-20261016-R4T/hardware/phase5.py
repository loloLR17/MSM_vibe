from hardware import *
local=P.parent;check=json.loads((P/'phase2b-comparison.json').read_text());assert check['full_exact_model'] and check['protected_exact']
assert (P/'fram-after-maintenance-b.bin').read_bytes()==(local/'model-renew.bin').read_bytes()
elf=local/'probe-build-02/tr2-r4t-monotone-recovery-NON-PRODUCTION.elf'
debug('phase5-composed-result',elf,IDENTITY+'''p r4t_recovery
p engine.status
p store.generation
if r4t_recovery.stage != 4 || r4t_recovery.error || r4m_guard.phase != 3 || store.generation != 30 || store.epoch.current != 5 || engine.has_active || owner.acquisition.fifo.running
 quit 4
end
break r4r_checkpoint
set r4s_continue_token=0x53345033
set r4_trigger=0x5234000E
continue
p r4t_recovery
p r4s_result
p r4r_result
p r4_result
p work
p r4s_max_service
p event_time
p metric
p r4r_first_value
p engine
p store.generation
p store.epoch
p store.effect
p live.generation
p owner.acquisition.fifo
p r4m_range_count
p r4m_op_count
p r4m_ranges
printf "FIRST_RMS=%u,%u,%u,%u PEAK=%u,%u,%u,%u N=%u DURATION=%u\\n",r4r_first_value.rms_global_mg,r4r_first_value.rms_x_mg,r4r_first_value.rms_y_mg,r4r_first_value.rms_z_mg,r4r_first_value.peak_global_mg,r4r_first_value.peak_x_mg,r4r_first_value.peak_y_mg,r4r_first_value.peak_z_mg,r4r_first_value.valid_sample_count,r4r_first_value.window_duration_ms
'''+f'''if r4r_result.raw_words > 0
dump binary memory {win(P/'phase5-raw.bin')} r4r_raw r4r_raw+7*r4r_result.raw_words
end
dump binary memory {win(P/'phase5-xyz.bin')} r4r_xyz (char*)r4r_xyz+sizeof(r4r_xyz)
if r4s_result.trace_count > 0
dump binary memory {win(P/'phase5-trace.bin')} r4s_trace &r4s_trace[r4s_result.trace_count]
end
dump binary memory {win(P/'phase5-valid-adu.bin')} r4r_valid_adu r4r_valid_adu+101
dump binary memory {win(P/'phase5-held-adu.bin')} r4r_held_adu r4r_held_adu+101
set $r=(int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r
if $r != 0
 quit 6
end
dump binary memory {win(P/'fram-after-composed.bin')} baseline baseline+262144
''',timeout=600)
print('COMPOSED TERMINAL HARVESTED; inspect verdict before any further action',flush=True)
