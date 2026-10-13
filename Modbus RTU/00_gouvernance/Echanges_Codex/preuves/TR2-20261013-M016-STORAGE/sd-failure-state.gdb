set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p hsd2.State
p/x hsd2.ErrorCode
p sd
p store.recovery_required
p store.active_logical_bytes
p store.active_durable_bytes
p store.buffered_bytes
p store.writer
p/x hsd2.Instance->STA
p/x hsd2.Instance->CMD
p/x hsd2.Instance->RESP1
p/x hsd2.Instance->CLKCR
p (unsigned int)HAL_SD_GetCardState(&hsd2)
p/x hsd2.ErrorCode
p/x hsd2.Instance->RESP1

detach
quit
