set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p (unsigned int)HAL_SD_ConfigWideBusOperation(&hsd2,0)
p/x hsd2.Instance->CLKCR
p/x hsd2.ErrorCode
p (unsigned int)HAL_SD_GetCardState(&hsd2)
p (unsigned int)campaign_data_store_bulk_init(&store,&region.interface,payload,4096,scratch,4132)
p (unsigned int)recover_saved()
p storage_qual_recovery_status
p storage_qual_recovery_bytes
p store.recovery_required

detach
quit
