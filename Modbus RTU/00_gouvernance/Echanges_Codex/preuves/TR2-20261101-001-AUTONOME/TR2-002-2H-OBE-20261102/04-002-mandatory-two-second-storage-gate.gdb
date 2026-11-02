set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if r3_result.error || r3_result.stage!=4 || r3_result.run!=1 || fifo.running || r3_writer.active
 quit 12
end
thbreak r3_finished
set var r3_command=0x44335203
continue
delete breakpoints
p r3_result
p r3_writer
p durable_status
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\gate\result.bin &r3_result (char *)&r3_result+sizeof(r3_result)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\gate\source-sha.bin &r3_source_sha (char *)&r3_source_sha+sizeof(r3_source_sha)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\gate\header.bin &r3_header (char *)&r3_header+sizeof(r3_header)
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-002-2H-OBE-20261102\gate\footer.bin &durable_footer (char *)&durable_footer+sizeof(durable_footer)
if r3_result.error || r3_result.stage!=4 || r3_result.run!=3 || fifo.running || r3_writer.active || r3_writer.uncertain || r3_writer.needs_power_cycle
 quit 13
end
set $gate_verify=(int)durable_verify_last(r3_source_sha)
printf "GATE_FILE_DIGEST_VERIFY=%d\n",$gate_verify
if $gate_verify
 quit 14
end
p r3_clock_fault
p/x *(unsigned int *)0x42021414

detach
quit
