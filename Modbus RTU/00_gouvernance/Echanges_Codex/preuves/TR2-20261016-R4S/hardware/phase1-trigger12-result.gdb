set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4s_result
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
printf "FINAL_STAGE=%u ERROR=%u CAL_ERROR=%u\n",r4s_result.stage,r4s_result.error,r4r_result.error
printf "FIRST_RMS=%u,%u,%u,%u PEAK=%u,%u,%u,%u N=%u DURATION=%u\n",r4r_first_value.rms_global_mg,r4r_first_value.rms_x_mg,r4r_first_value.rms_y_mg,r4r_first_value.rms_z_mg,r4r_first_value.peak_global_mg,r4r_first_value.peak_x_mg,r4r_first_value.peak_y_mg,r4r_first_value.peak_z_mg,r4r_first_value.valid_sample_count,r4r_first_value.window_duration_ms
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4s\hardware\phase1-raw.bin r4r_raw r4r_raw+7*r4r_result.raw_words
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4s\hardware\phase1-brackets.bin r4r_brackets &r4r_brackets[r4r_result.brackets]
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4s\hardware\phase1-xyz.bin r4r_xyz (char*)r4r_xyz+sizeof(r4r_xyz)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4s\hardware\phase1-trace.bin r4s_trace &r4s_trace[r4s_result.trace_count]
set $r=(int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r
if $r != 0
 quit 6
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-r4s\hardware\fram-after-phase1.bin baseline baseline+262144

detach
quit
