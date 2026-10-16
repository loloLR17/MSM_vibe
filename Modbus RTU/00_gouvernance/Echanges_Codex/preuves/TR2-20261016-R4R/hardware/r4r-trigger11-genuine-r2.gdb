set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4r_result
p engine
p store.epoch
p store.effect
if r4r_result.stage != 11 || r4r_result.error != 0 || !diag_fifo.stopped || owner.acquisition.fifo.running || store.epoch.current != 4 || store.effect.run_id != 4
 quit 5
end
break r4r_checkpoint
set r4_trigger=0x5234000B
continue
p r4r_result
p r4_result
p work
p event_time
p metric
p r4r_first_value
p engine
p store.epoch
p store.effect
p r4m_range_count
p r4m_ranges
printf "FINAL_STAGE=%u ERROR=%u\n",r4r_result.stage,r4r_result.error
printf "FIRST_RMS=%u,%u,%u,%u PEAK=%u,%u,%u,%u N=%u DURATION=%u\n",r4r_first_value.rms_global_mg,r4r_first_value.rms_x_mg,r4r_first_value.rms_y_mg,r4r_first_value.rms_z_mg,r4r_first_value.peak_global_mg,r4r_first_value.peak_x_mg,r4r_first_value.peak_y_mg,r4r_first_value.peak_z_mg,r4r_first_value.valid_sample_count,r4r_first_value.window_duration_ms
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4r\hardware\xyz.bin r4r_xyz (char*)r4r_xyz+sizeof(r4r_xyz)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4r\hardware\valid-adu.bin r4r_valid_adu r4r_valid_adu+101
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4r\hardware\held-adu.bin r4r_held_adu r4r_held_adu+101
set $read=(int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $read
if $read != 0
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4r\hardware\fram-after-live.bin baseline baseline+262144

detach
quit
