set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
if *(unsigned int*)0x0BFA0700 != 0x004A003D || *(unsigned int*)0x0BFA0704 != 0x38425018 || *(unsigned int*)0x0BFA0708 != 0x20333157
 echo UNSAFE_AUDIT_REFUSED\n
 detach
 quit 7
end
if owner.acquisition.fifo.running || engine.has_active || r4u_result.error != 0 || engine.state != LIVE_READY || engine.last.status != 4 || engine.last.result != 0 || engine.last.request.command.identity.command_code != 14 || engine.last.request.epoch != 5 || engine.last.request.command.transaction_id != 17
 echo UNSAFE_AUDIT_REFUSED\n
 detach
 quit 7
end
p r4u_result
p engine.last
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-live-com4-loss-diag\safe-audit-before-commands\flash-prefix.bin 0x08000000 0x08023E50

detach
quit
