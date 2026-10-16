set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
set print elements 300
printf "R4M_LABEL r4_result\n"
p r4_result
printf "R4M_LABEL work\n"
p work
printf "R4M_LABEL r4m_guard\n"
p r4m_guard
printf "R4M_LABEL r4m_range_count\n"
p r4m_range_count
printf "R4M_LABEL r4m_op_count\n"
p r4m_op_count
printf "R4M_LABEL r4m_old_rejected\n"
p r4m_old_rejected
printf "R4M_LABEL r4m_protected_checks\n"
p r4m_protected_checks
printf "R4M_LABEL r4m_cs_closed\n"
p r4m_cs_closed
printf "R4M_LABEL r4m_fifo_max_all\n"
p r4m_fifo_max_all
printf "R4M_LABEL r4m_xyz_all\n"
p r4m_xyz_all
printf "R4M_LABEL r4m_ts_all\n"
p r4m_ts_all
printf "R4M_LABEL r4m_stop_max_us\n"
p r4m_stop_max_us
printf "R4M_LABEL r4m_stop_end_us\n"
p r4m_stop_end_us
printf "R4M_LABEL r4m_fault_seen_us\n"
p r4m_fault_seen_us
printf "R4M_LABEL r4m_fault_reaction_us\n"
p r4m_fault_reaction_us
printf "R4M_LABEL bench_log_refused\n"
p bench_log_refused
printf "R4M_LABEL bench_preclose_overflow\n"
p bench_preclose_overflow
printf "R4M_LABEL engine\n"
p engine
printf "R4M_LABEL store.epoch\n"
p store.epoch
printf "R4M_LABEL store.effect\n"
p store.effect
printf "R4M_LABEL configuration.active\n"
p configuration.active
printf "R4M_LABEL owner.acquisition.fifo\n"
p owner.acquisition.fifo
if r4m_range_count > 0
printf "R4M_LABEL *r4m_ranges@r4m_range_count\n"
p *r4m_ranges@r4m_range_count
end
if r4m_op_count > 0
printf "R4M_LABEL *r4m_operations@r4m_op_count\n"
p *r4m_operations@r4m_op_count
end
detach
quit
