set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p storage_qual_stage
p storage_qual_result
p storage_qual_samples
p storage_qual_checkpoints
p storage_qual_append_ms
p storage_qual_max_call_ms
p storage_qual_capacity_mb
p storage_qual_configuration_available
p storage_qual_configuration_generation
p storage_qual_recovery_status
p storage_qual_recovery_bytes
p config_service.has_active
p config_service.active
p fram_media.physical.write
p SystemCoreClock
x/2wx 0xE000ED28

detach
quit
